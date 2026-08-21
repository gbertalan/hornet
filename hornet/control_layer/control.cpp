#include "control.h"
#include "model_layer/editormodel.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/terminalmodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/gridservice.h"
#include "service_layer/terminalservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/boxselecteddto.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"

#include "shared/dto_view_to_model/editorvisiblelinesdto.h"
#include <shared/dto_model_to_view/editorviewstatedto.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include "shared/dto_model_to_view//boxlistpagerequestdto.h"
#include "shared/dto_model_to_view/boxlistpagedto.h"

#include "shared/dto_view_to_model/filepathlistdto.h"

#include "shared/dto_view_to_model/boxunloadrequesteddto.h"
#include "shared/dto_view_to_model/toolbuttonactivateddto.h"

#include "service_layer/miresultparser.h"
#include "service_layer/toolscriptparser.h"
#include "shared/dto_model_to_view/toollistsourcedto.h"
#include "shared/dto_model_to_view/toolsourcedto.h"
#include "shared/dto_model_to_view/tooltrustpromptdto.h"
#include "shared/dto_view_to_model/editorselectiondto.h"
#include "shared/dto_view_to_model/tooltextfieldcommitdto.h"

// ================================================================
// SLICE: construction & initialization
// ================================================================

Control::Control(IModelAccessRead &modelAccess,
                 WindowService &windowService,
                 EditorService &editorService,
                 TerminalService &terminalService,
                 GridService &gridService,
                 View &view)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
    , m_gridService(gridService)
    , m_windowControl(modelAccess, windowService, view)
    , m_editorControl(modelAccess, editorService, view)
    , m_terminalControl(modelAccess, editorService, terminalService)
    , m_gridControl(modelAccess, gridService, view)
    , m_gdbControl()
    , m_toolControl(gridService, m_gdbControl)
{
    connect(&m_toolControl, &ToolControl::sourceValueUpdated, this, [this]() {
        m_gridControl.sendViewStateToGrid();
    });
    connect(&m_gdbControl,
            &GdbControl::commandCompleted,
            this,
            [this](const QString &commandText, const QString &resultText) {
                appendToLogBox(commandText, resultText);
                m_gridControl.sendViewStateToGrid();
            });
    connect(&m_gdbControl, &GdbControl::asyncNotificationReceived, this, [this](const QString &line) {
        appendToLogBox("gdb (async)", line);
        m_gridControl.sendViewStateToGrid();
    });
    connect(&m_gdbControl, &GdbControl::sessionEnded, this, [this](const QString &reason) {
        appendToLogBox("gdb (session)", reason);
        m_gridControl.sendViewStateToGrid();
    });
    connect(&m_gdbControl,
            &GdbControl::rawListResultReceived,
            this,
            [this](const QString &listName,
                   const QString &resultText,
                   const QStringList &registerNames) {
                const std::vector<QString> rows = MiResultParser::parseRows(resultText,
                                                                            registerNames);
                QVector<QString> rowsAsQVector;
                rowsAsQVector.reserve(static_cast<int>(rows.size()));
                for (const QString &row : rows)
                    rowsAsQVector.push_back(row);
                m_gridService.upsertListBox(listName, rowsAsQVector);
                m_gridControl.sendViewStateToGrid();
            });
}

void Control::init()
{
    m_windowControl.init();
    m_editorControl.init();
    m_terminalControl.init();
    m_gridControl.init();

    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();

    dispatchHornetCommand(HornetCommandDTO{true, "load", "FILE_LOADER.tool", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setpos", "last 3 20", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setsize", "last 26 20", workingDir});

    dispatchHornetCommand(HornetCommandDTO{true, "load", "RESTORE_SESSION.tool", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setpos", "last 3 45", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setsize", "last 20 18", workingDir});

    dispatchHornetCommand(HornetCommandDTO{true, "load", "gdbtest.tool", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setpos", "last 3 65", workingDir});
    dispatchHornetCommand(HornetCommandDTO{true, "setsize", "last 40 36", workingDir});

    if (!m_recentlyCreatedBoxIds.empty())
        m_gridService.storeToolFieldValue(m_recentlyCreatedBoxIds.back(),
                                          "sessionpath",
                                          QString::fromStdString(sessionScriptPath().string()));

    m_gridControl.sendViewStateToGrid();

    const int terminalBoxId = m_gridService.retrieveFirstBoxIdOfType(BoxContentType::Terminal);
    if (terminalBoxId != -1) {
        m_editorService.setIsTerminal(true);
        m_editorControl.sendStateToEditor(buildTerminalPrompts());
        m_editorControl.sendSettingsToEditor();
        m_currentlySelectedBoxId = terminalBoxId;
        m_gridService.storeSelectedBox(terminalBoxId);
        m_gridControl.sendViewStateToGrid();
        m_windowControl.sendFileNameToTitlebar(
            m_gridService.retrieveBoxContent(terminalBoxId).headerText);
        m_windowControl.sendCurrentBoxIdToTitlebar(terminalBoxId);
    }
}
// ================================================================
// SLICE: window
// ================================================================

void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowControl.dispatchWindowStateChanged(dto);
}

// ================================================================
// SLICE: editor <-> model sync (state/cursor)
// ================================================================

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});

    if (m_currentlySelectedBoxId != -1 && dto.topLineIndex != m_lastSyncedBoxScrollOffset) {
        m_gridService.storeBoxScrollOffset(m_currentlySelectedBoxId, dto.topLineIndex);
        m_lastSyncedBoxScrollOffset = dto.topLineIndex;
        m_gridControl.sendViewStateToGrid();
    }
}

void Control::onEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    m_editorService.storeCursorPos(dto);
    if (m_modelAccess.getEditorModel().isTerminal() && !m_isRestoringBoxState)
        m_terminalControl.dispatchEditorCursorPosChanged(dto);
    m_editorControl.sendCursorPosToEditor();

    if (m_currentlySelectedBoxId != -1 && !m_isRestoringBoxState) {
        flushEditorContentToBox(m_currentlySelectedBoxId);
        m_gridControl.sendViewStateToGrid();
    }
}

void Control::onEditorSelectionChanged(const EditorSelectionDTO &dto)
{
    m_editorService.storeSelection(dto);
    if (m_currentlySelectedBoxId != -1 && !m_isRestoringBoxState) {
        m_gridService.storeBoxSelection(m_currentlySelectedBoxId,
                                        dto.anchorX,
                                        dto.anchorY,
                                        dto.extentX,
                                        dto.extentY,
                                        dto.hasSelection);
        m_gridControl.sendViewStateToGrid();
    }
}
// ================================================================
// SLICE: editor key dispatch (typing, plus terminal/hornet routing)
// ================================================================

void Control::onEditorKeyPressed(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlD) {
        onDebugRequested();
        return;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlA) {
        m_editorControl.selectAll();
        if (m_currentlySelectedBoxId != -1) {
            flushEditorContentToBox(m_currentlySelectedBoxId);
            if (!m_isRestoringBoxState) {
                m_gridService
                    .storeBoxSelection(m_currentlySelectedBoxId,
                                       m_modelAccess.getEditorModel().getSelectionAnchorX(),
                                       m_modelAccess.getEditorModel().getSelectionAnchorY(),
                                       m_modelAccess.getEditorModel().getSelectionExtentX(),
                                       m_modelAccess.getEditorModel().getSelectionExtentY(),
                                       m_modelAccess.getEditorModel().hasSelection());
            }
            m_gridControl.sendViewStateToGrid();
        }
        m_editorControl.sendCursorPosToEditor();
        m_editorControl.sendSelectionToEditor();
        return;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlC) {
        m_editorControl.copySelection();
        return;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlV) {
        int lineCountBefore = m_modelAccess.getEditorModel().getNoOfLines();
        int cursorYBefore = m_modelAccess.getEditorModel().getCursorY();
        m_editorControl.pasteFromClipboard();
        if (m_modelAccess.getEditorModel().isTerminal())
            m_terminalControl.removePromptForDeletedLine(lineCountBefore, cursorYBefore);
        if (m_currentlySelectedBoxId != -1) {
            flushEditorContentToBox(m_currentlySelectedBoxId);
            if (!m_isRestoringBoxState) {
                m_gridService.storeBoxSelection(m_currentlySelectedBoxId, 0, 0, 0, 0, false);
            }
            m_gridControl.sendViewStateToGrid();
        }
        m_editorControl.sendStateToEditor(m_modelAccess.getEditorModel().isTerminal()
                                              ? buildTerminalPrompts()
                                              : QVector<QString>{});
        m_editorControl.sendCursorPosToEditor();
        m_editorControl.sendSelectionToEditor();
        return;
    }
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlX) {
        int lineCountBefore = m_modelAccess.getEditorModel().getNoOfLines();
        int cursorYBefore = m_modelAccess.getEditorModel().getCursorY();
        m_editorControl.cutSelection();
        if (m_modelAccess.getEditorModel().isTerminal())
            m_terminalControl.removePromptForDeletedLine(lineCountBefore, cursorYBefore);
        if (m_currentlySelectedBoxId != -1) {
            flushEditorContentToBox(m_currentlySelectedBoxId);
            if (!m_isRestoringBoxState) {
                m_gridService.storeBoxSelection(m_currentlySelectedBoxId, 0, 0, 0, 0, false);
            }
            m_gridControl.sendViewStateToGrid();
        }
        m_editorControl.sendStateToEditor(m_modelAccess.getEditorModel().isTerminal()
                                              ? buildTerminalPrompts()
                                              : QVector<QString>{});
        m_editorControl.sendCursorPosToEditor();
        m_editorControl.sendSelectionToEditor();
        return;
    }
    if (m_modelAccess.getEditorModel().isTerminal()) {
        const TerminalKeyPressResultDTO result = m_terminalControl.dispatchTerminalKeyPress(dto);
        if (result.wasHandled) {
            const CommandExecutionResultDTO &executionResult = result.commandExecutionResult;
            if (!executionResult.commandText.isEmpty()) {
                if (executionResult.hornetCommand.wasHornetCommand) {
                    const QString hornetMessage = dispatchHornetCommand(
                        executionResult.hornetCommand);
                    if (!hornetMessage.isEmpty())
                        appendToLogBox(executionResult.commandText, hornetMessage);
                } else {
                    if (executionResult.shellOutput.isEmpty())
                        appendToLogBox(executionResult.commandText, executionResult.shellOutput);
                    else
                        createCommandOutputBox(executionResult.commandText,
                                               executionResult.shellOutput);
                }
            }
            if (m_currentlySelectedBoxId != -1)
                flushEditorContentToBox(m_currentlySelectedBoxId);
            m_gridControl.sendViewStateToGrid();
            m_editorControl.sendStateToEditor(buildTerminalPrompts());
            m_editorControl.sendCursorPosToEditor();
            return;
        }
    }
    int lineCountBefore = m_modelAccess.getEditorModel().getNoOfLines();
    int cursorYBefore = m_modelAccess.getEditorModel().getCursorY();
    m_editorControl.dispatchEditorKeyPress(dto);
    if (m_modelAccess.getEditorModel().isTerminal())
        m_terminalControl.removePromptForDeletedLine(lineCountBefore, cursorYBefore);
    if (m_currentlySelectedBoxId != -1) {
        flushEditorContentToBox(m_currentlySelectedBoxId);
        if (!m_isRestoringBoxState) {
            m_gridService.storeBoxSelection(m_currentlySelectedBoxId,
                                            m_modelAccess.getEditorModel().getSelectionAnchorX(),
                                            m_modelAccess.getEditorModel().getSelectionAnchorY(),
                                            m_modelAccess.getEditorModel().getSelectionExtentX(),
                                            m_modelAccess.getEditorModel().getSelectionExtentY(),
                                            m_modelAccess.getEditorModel().hasSelection());
        }
        m_gridControl.sendViewStateToGrid();
    }
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});
    m_editorControl.sendCursorPosToEditor();
    m_editorControl.sendSelectionToEditor();
}

// ================================================================
// SLICE: grid viewport (zoom, pan)
// ================================================================

void Control::onGridZoomChanged(const GridZoomDTO &dto)
{
    m_gridControl.dispatchGridZoomChange(dto);
}

void Control::onGridDrag(const GridDragDTO &dto)
{
    m_gridControl.dispatchGridDrag(dto);
}

// ================================================================
// SLICE: box manipulation (drag)
// ================================================================

void Control::onBoxDragged(const BoxDragDTO &dto)
{
    m_gridControl.dispatchBoxDrag(dto);
}

// ================================================================
// SLICE: terminal prompt helper (used by editor sync + box selection)
// ================================================================

QVector<QString> Control::buildTerminalPrompts() const
{
    QVector<QString> terminalPrompts;
    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_modelAccess.getTerminalModel().retrieveTerminalPromptAndDirs();
    for (const TerminalPromptAndDir &line : terminalPromptAndDirs)
        terminalPrompts.push_back(
            QString::fromUcs4(reinterpret_cast<const char32_t *>(line.prompt.c_str()),
                              static_cast<int>(line.prompt.size())));
    return terminalPrompts;
}

// ================================================================
// SLICE: box selection
// ================================================================

void Control::onBoxSelected(const BoxSelectedDTO &dto)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    const HornetCommandDTO command{true, "select", QString::number(dto.boxId), workingDir};
    const QString message = dispatchHornetCommand(command);
    if (!message.isEmpty())
        appendToLogBox("select " + QString::number(dto.boxId), message);
}

// ================================================================
// SLICE: box manipulation (resize)
// ================================================================

void Control::onBoxResized(const BoxResizeDTO &dto)
{
    m_gridControl.dispatchBoxResize(dto);
}

// ================================================================
// SLICE: editor <-> box sync helpers
// ================================================================

void Control::flushEditorContentToBox(int boxId)
{
    if (m_gridService.retrieveBoxContentType(boxId) == BoxContentType::List)
        return;
    const std::vector<std::u32string> &currentLines = m_modelAccess.getEditorModel().getTextLines();
    QVector<QString> linesAsQString;
    linesAsQString.reserve(static_cast<int>(currentLines.size()));
    for (const std::u32string &line : currentLines)
        linesAsQString.push_back(convertU32StringToQString(line));
    const int cursorX = m_modelAccess.getEditorModel().getCursorX();
    const int cursorY = m_modelAccess.getEditorModel().getCursorY();
    m_gridService.storeBoxContent(boxId, linesAsQString, cursorX, cursorY);

    const std::vector<MarkRange> &marks = m_modelAccess.getEditorModel().getMarks();
    QVector<MarkRange> marksAsQVector(marks.begin(), marks.end());
    m_gridService.storeBoxMarks(boxId, marksAsQVector);
}

// ================================================================
// SLICE: type conversion helpers (u32string <-> QString)
// ================================================================

QString Control::convertU32StringToQString(const std::u32string &text) const
{
    return QString::fromUcs4(reinterpret_cast<const char32_t *>(text.c_str()),
                             static_cast<int>(text.size()));
}

std::u32string Control::convertQStringToU32String(const QString &text) const
{
    const QVector<uint> ucs4 = text.toUcs4();
    return std::u32string(ucs4.begin(), ucs4.end());
}

std::vector<std::u32string> Control::convertBodyLinesToU32(const QVector<QString> &bodyLines) const
{
    std::vector<std::u32string> result;
    result.reserve(bodyLines.size());
    for (const QString &line : bodyLines)
        result.push_back(convertQStringToU32String(line));
    return result;
}

// ================================================================
// SLICE: hornet command system - file loading helper
// ================================================================

bool Control::loadFileIntoNewBox(const std::filesystem::path &filePath)
{
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cout << "hornet load: could not open file: " << filePath.string() << std::endl;
        return false;
    }

    QVector<QString> bodyLines;
    std::string line;
    while (std::getline(fileStream, line))
        bodyLines.push_back(QString::fromStdString(line));

    const QString headerText = QString::fromStdString(filePath.filename().string());
    const QString originFilePath = QString::fromStdString(filePath.string());
    m_recentlyCreatedBoxIds.push_back(
        m_gridService.addBox(0, 0, 20, 15, headerText, bodyLines, true, originFilePath));
    return true;
}

// ================================================================
// SLICE: hornet command system - main dispatch
// ================================================================

QString Control::dispatchHornetCommand(const HornetCommandDTO &command)
{
    if (command.subcommand == "load") {
        const std::filesystem::path filePath = command.workingDirectory
                                               / command.argument.toStdString();
        if (loadFileIntoNewBox(filePath)) {
            m_gridControl.sendViewStateToGrid();
            return "loaded file: " + QString::fromStdString(filePath.filename().string());
        }
        return "could not open file: " + QString::fromStdString(filePath.string());
    }

    if (command.subcommand == "load-dir") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            return "usage: hornet load-dir <path> <extension> [recursive]";

        const std::filesystem::path dirPath = command.workingDirectory / parts.at(0).toStdString();
        const std::string extensionWithDot = "." + parts.at(1).toStdString();
        const bool recursive = (parts.size() >= 3 && parts.at(2) == "recursive");

        if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
            return "not a directory: " + QString::fromStdString(dirPath.string());

        int filesLoaded = 0;
        if (recursive) {
            for (const auto &entry : std::filesystem::recursive_directory_iterator(dirPath))
                if (entry.is_regular_file() && entry.path().extension() == extensionWithDot)
                    if (loadFileIntoNewBox(entry.path()))
                        ++filesLoaded;
        } else {
            for (const auto &entry : std::filesystem::directory_iterator(dirPath))
                if (entry.is_regular_file() && entry.path().extension() == extensionWithDot)
                    if (loadFileIntoNewBox(entry.path()))
                        ++filesLoaded;
        }

        m_gridControl.sendViewStateToGrid();
        return "loaded " + QString::number(filesLoaded) + " file(s)";
    }

    if (command.subcommand == "setpos") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setpos <boxId> <x> <y>";

        int boxId = -1;
        bool xOk = false, yOk = false;
        const bool boxIdOk = resolveBoxIdToken(parts.at(0), boxId);
        const int x = parts.at(1).toInt(&xOk);
        const int y = parts.at(2).toInt(&yOk);
        if (!boxIdOk || !xOk || !yOk)
            return "usage: hornet setpos <boxId|last> <x> <y>";

        try {
            m_gridService.storeBoxPosition(boxId, x, y);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }

        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "setsize") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setsize <boxId> <width> <height>";
        int boxId = -1;
        bool widthOk = false, heightOk = false;
        const bool boxIdOk = resolveBoxIdToken(parts.at(0), boxId);
        const int width = parts.at(1).toInt(&widthOk);
        const int height = parts.at(2).toInt(&heightOk);
        if (!boxIdOk || !widthOk || !heightOk)
            return "usage: hornet setsize <boxId|last> <width> <height>";

        try {
            m_gridService.storeBoxSize(boxId, width, height);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "setscroll") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            return "usage: hornet setscroll <boxId> <offset>";
        int boxId = -1;
        bool offsetOk = false;
        const bool boxIdOk = resolveBoxIdToken(parts.at(0), boxId);
        const int offset = parts.at(1).toInt(&offsetOk);
        if (!boxIdOk || !offsetOk)
            return "usage: hornet setscroll <boxId|last> <offset>";
        try {
            m_gridService.storeBoxScrollOffset(boxId, offset);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "setcursor") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setcursor <boxId> <x> <y>";
        int boxId = -1;
        bool xOk = false, yOk = false;
        const bool boxIdOk = resolveBoxIdToken(parts.at(0), boxId);
        const int x = parts.at(1).toInt(&xOk);
        const int y = parts.at(2).toInt(&yOk);
        if (!boxIdOk || !xOk || !yOk)
            return "usage: hornet setcursor <boxId|last> <x> <y>";

        try {
            m_gridService.storeCursorPosition(boxId, x, y);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "setzoom") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 1)
            return "usage: hornet setzoom <zoomLevel>";
        bool zoomOk = false;
        const int zoomLevel = parts.at(0).toInt(&zoomOk);
        if (!zoomOk)
            return "usage: hornet setzoom <zoomLevel> (must be an integer)";
        m_gridService.storeZoomLevel(zoomLevel);
        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "setoffset") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            return "usage: hornet setoffset <x> <y>";
        bool xOk = false, yOk = false;
        const int x = parts.at(0).toInt(&xOk);
        const int y = parts.at(1).toInt(&yOk);
        if (!xOk || !yOk)
            return "usage: hornet setoffset <x> <y> (all must be integers)";
        m_gridService.storeGridOffset(x, y);
        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "unload") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty())
            return "usage: hornet unload <id|range> [<id|range> ...] (e.g. hornet unload 1-3 5 "
                   "7-9)";

        std::vector<int> idsToRemove;
        for (const QString &part : parts) {
            const int dashIndex = part.indexOf('-');
            if (dashIndex == -1) {
                int id = -1;
                if (!resolveBoxIdToken(part, id))
                    return "invalid id or range: " + part;
                idsToRemove.push_back(id);
            } else {
                bool startOk = false, endOk = false;
                const int start = part.left(dashIndex).toInt(&startOk);
                const int end = part.mid(dashIndex + 1).toInt(&endOk);
                if (!startOk || !endOk || start > end)
                    return "invalid id or range: " + part;
                for (int id = start; id <= end; ++id)
                    idsToRemove.push_back(id);
            }
        }

        QStringList notFound;
        QStringList protectedIds;
        for (const int id : idsToRemove) {
            try {
                const BoxContentDTO content = m_gridService.retrieveBoxContent(id);
                if (content.contentType == BoxContentType::Terminal) {
                    protectedIds.push_back(QString::number(id));
                    continue;
                }
                if (id == m_currentlySelectedBoxId) {
                    const int terminalBoxId = m_gridService.retrieveFirstBoxIdOfType(
                        BoxContentType::Terminal);
                    if (terminalBoxId != -1)
                        selectBox(terminalBoxId);
                }
                m_gridService.removeBox(id);
                const auto recentIt = std::find(m_recentlyCreatedBoxIds.begin(),
                                                m_recentlyCreatedBoxIds.end(),
                                                id);
                if (recentIt != m_recentlyCreatedBoxIds.end())
                    m_recentlyCreatedBoxIds.erase(recentIt);
            } catch (const std::runtime_error &) {
                notFound.push_back(QString::number(id));
            }
        }

        m_gridControl.sendViewStateToGrid();
        QStringList problems;
        if (!notFound.isEmpty())
            problems.push_back("no box(es) with id: " + notFound.join(", "));
        if (!protectedIds.isEmpty())
            problems.push_back("cannot unload terminal box: #" + protectedIds.join(", "));
        if (!problems.isEmpty())
            return problems.join("; ");
        return "";
    }

    if (command.subcommand == "run") {
        const std::filesystem::path scriptPath = command.workingDirectory
                                                 / command.argument.toStdString();
        return executeScriptFile(scriptPath, command.workingDirectory, 1);
    }

    if (command.subcommand == "save") {
        if (command.argument.isEmpty())
            return "usage: hornet save <file>";
        const std::filesystem::path saveFilePath = command.workingDirectory
                                                   / command.argument.toStdString();
        return saveProjectToFile(saveFilePath);
    }

    if (command.subcommand == "render") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty())
            return "usage: hornet render <boxId|last>";
        int boxId = -1;
        if (!resolveBoxIdToken(parts.at(0), boxId))
            return "usage: hornet render <boxId|last>";
        return m_toolControl.dispatchToolCommand(boxId, command.workingDirectory);
    }

    if (command.subcommand == "trust") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            return "usage: hornet trust <boxId|last> <sourceName>";
        int boxId = -1;
        if (!resolveBoxIdToken(parts.at(0), boxId))
            return "usage: hornet trust <boxId|last> <sourceName>";
        return m_toolControl.dispatchTrustCommand(boxId, parts.at(1), command.workingDirectory);
    }

    if (command.subcommand == "gdb") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty())
            return "usage: hornet gdb <start <binary>|stop|raw <mi-command>>";
        const QString action = parts.at(0);

        if (action == "start") {
            if (parts.size() < 2)
                return "usage: hornet gdb start <binary>";
            const QString binaryArg = command.argument.mid(action.length()).trimmed();
            const std::filesystem::path binaryPath = command.workingDirectory
                                                     / binaryArg.toStdString();
            return m_gdbControl.dispatchStart(QString::fromStdString(binaryPath.string()),
                                              command.workingDirectory);
        }
        if (action == "stop")
            return m_gdbControl.dispatchStop();
        if (action == "raw") {
            if (parts.size() < 2)
                return "usage: hornet gdb raw <mi-command>";
            return m_gdbControl.dispatchRaw(command.argument.mid(action.length()).trimmed());
        }
        if (action == "rawq") {
            if (parts.size() < 2)
                return "usage: hornet gdb rawq <mi-command>";
            return m_gdbControl.dispatchRawDebugPrint(
                command.argument.mid(action.length()).trimmed());
        }
        if (action == "list") {
            if (parts.size() < 3)
                return "usage: hornet gdb list <name> <mi-command>";
            const QString listName = parts.at(1);
            const QString miCommand = QStringList(parts.mid(2)).join(' ');
            return m_gdbControl.dispatchRawToList(listName, miCommand);
        }
        return "unknown gdb action: " + action;
    }

    if (command.subcommand == "select") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty())
            return "usage: hornet select <boxId|last>";
        int boxId = -1;
        if (!resolveBoxIdToken(parts.at(0), boxId))
            return "usage: hornet select <boxId|last>";
        try {
            selectBox(boxId);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        return "";
    }

    if (command.subcommand == "mark") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet mark <boxId|last> <startLine> <endLine>";
        int boxId = -1;
        bool startOk = false, endOk = false;
        const bool boxIdOk = resolveBoxIdToken(parts.at(0), boxId);
        const int startLine = parts.at(1).toInt(&startOk) - 1;
        const int endLine = parts.at(2).toInt(&endOk) - 1;
        if (!boxIdOk || !startOk || !endOk || startLine > endLine || startLine < 0)
            return "usage: hornet mark <boxId|last> <startLine> <endLine> (1-indexed)";

        try {
            QVector<MarkRange> marks = m_gridService.retrieveBoxContent(boxId).marks;
            marks.push_back(MarkRange(startLine, endLine));
            m_gridService.storeBoxMarks(boxId, marks);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }

        if (boxId == m_currentlySelectedBoxId) {
            const QVector<MarkRange> updatedMarks = m_gridService.retrieveBoxContent(boxId).marks;
            std::vector<MarkRange> marksAsStdVector(updatedMarks.begin(), updatedMarks.end());
            m_editorService.storeMarks(marksAsStdVector);
            m_editorControl.sendMarksToEditor();
        }

        m_gridControl.sendViewStateToGrid();
        return "";
    }

    if (command.subcommand == "noop") {
        return "";
    }

    if (command.subcommand == "exit") {
        emit exitRequested();
        return "";
    }

    if (command.subcommand == "autosave") {
        std::filesystem::create_directories(sessionDataDirectory());
        return saveProjectToFile(sessionScriptPath());
    }

    return "unknown hornet command: " + command.subcommand;
}

// ================================================================
// SLICE: hornet command system - script execution
// ================================================================

QString Control::executeScriptFile(const std::filesystem::path &filePath,
                                   const std::filesystem::path &workingDir,
                                   int depth)
{
    constexpr int maxScriptDepth = 10;
    if (depth > maxScriptDepth)
        return "script recursion too deep (max " + QString::number(maxScriptDepth)
               + "): " + QString::fromStdString(filePath.string());

    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
        return "could not open script file: " + QString::fromStdString(filePath.string());

    std::filesystem::path currentDir = filePath.parent_path();
    QStringList problems;
    std::string lineStr;
    while (std::getline(fileStream, lineStr)) {
        const QString line = QString::fromStdString(lineStr).trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const HornetCommandDTO hornetCommand = m_terminalControl.parseHornetCommand(line,
                                                                                    currentDir);
        if (hornetCommand.wasHornetCommand) {
            if (hornetCommand.subcommand == "run") {
                const std::filesystem::path nestedScriptPath
                    = currentDir / hornetCommand.argument.toStdString();
                const QString result = executeScriptFile(nestedScriptPath, currentDir, depth + 1);
                if (!result.isEmpty())
                    problems.push_back(result);
            } else {
                const QString message = dispatchHornetCommand(hornetCommand);
                if (!message.isEmpty())
                    problems.push_back(message);
            }
        } else {
            std::filesystem::path resultingDir;
            const QString output = m_terminalControl.runShellCommandInDirectory(line,
                                                                                currentDir,
                                                                                resultingDir);
            currentDir = resultingDir;
            if (output.isEmpty())
                appendToLogBox(line, output);
            else
                createCommandOutputBox(line, output);
        }
    }

    m_gridControl.sendViewStateToGrid();
    return problems.join("; ");
}

std::filesystem::path Control::sessionDataDirectory() const
{
    return std::filesystem::path(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
}

std::filesystem::path Control::sessionScriptPath() const
{
    return sessionDataDirectory() / "last_session.script";
}

// ================================================================
// SLICE: hornet command system - output box helper
// ================================================================

void Control::createCommandOutputBox(const QString &commandText, const QString &outputText)
{
    QVector<QString> bodyLines;
    if (outputText.isEmpty()) {
        bodyLines.push_back("(no output)");
    } else {
        for (const QString &line : outputText.split('\n'))
            bodyLines.push_back(line);
    }
    m_recentlyCreatedBoxIds.push_back(
        m_gridService.addBox(0, 0, 20, 15, commandText, bodyLines, false, QString()));
}

void Control::appendToLogBox(const QString &commandText, const QString &outputText)
{
    const int logBoxId = m_gridService.appendToLogBox(commandText, outputText);
    if (logBoxId == m_currentlySelectedBoxId) {
        const BoxContentDTO boxContent = m_gridService.retrieveBoxContent(logBoxId);
        const std::vector<std::u32string> bodyLinesAsU32 = convertBodyLinesToU32(
            boxContent.bodyLines);
        m_editorService.storeTextLines(bodyLinesAsU32, "txt");
        m_editorControl.sendStateToEditor();
    }
}

// ================================================================
// SLICE: hornet command system - "last" id resolution helper
// ================================================================

bool Control::resolveBoxIdToken(const QString &token, int &outBoxId) const
{
    if (token == "last") {
        if (m_recentlyCreatedBoxIds.empty())
            return false;
        outBoxId = m_recentlyCreatedBoxIds.back();
        return true;
    }
    bool ok = false;
    const int parsed = token.toInt(&ok);
    if (!ok)
        return false;
    outBoxId = parsed;
    return true;
}

void Control::selectBox(int boxId)
{
    const int previouslySelectedBoxId = m_currentlySelectedBoxId;
    if (previouslySelectedBoxId != -1)
        flushEditorContentToBox(previouslySelectedBoxId);
    m_currentlySelectedBoxId = boxId;
    m_lastSyncedBoxScrollOffset = -1;
    m_isRestoringBoxState = true;
    const BoxContentDTO boxContent = m_gridService.retrieveBoxContent(boxId);
    const std::vector<std::u32string> bodyLinesAsU32 = convertBodyLinesToU32(boxContent.bodyLines);
    if (boxContent.contentType == BoxContentType::Terminal) {
        m_editorService.storeTextLines(bodyLinesAsU32, "txt");
        m_editorService.setIsTerminal(true);
        m_editorControl.sendStateToEditor(buildTerminalPrompts());
    } else {
        m_editorService.storeTextLines(bodyLinesAsU32, "txt");
        m_editorService.setIsTerminal(false);
        m_editorControl.sendStateToEditor();
    }
    m_editorService.storeCursorPos(EditorCursorPosDTO(boxContent.cursorX, boxContent.cursorY));
    m_editorControl.sendCursorPosToEditor();

    m_editorService.storeSelection(EditorSelectionDTO{boxContent.selectionAnchorX,
                                                      boxContent.selectionAnchorY,
                                                      boxContent.selectionExtentX,
                                                      boxContent.selectionExtentY,
                                                      boxContent.hasSelection});
    m_editorControl.sendSelectionToEditor();

    const QVector<MarkRange> &boxMarks = boxContent.marks;
    std::vector<MarkRange> marksAsStdVector(boxMarks.begin(), boxMarks.end());
    m_editorService.storeMarks(marksAsStdVector);
    m_editorControl.sendMarksToEditor();

    m_editorControl.sendSettingsToEditor();
    m_windowControl.sendFileNameToTitlebar(boxContent.headerText);
    m_windowControl.sendCurrentBoxIdToTitlebar(boxId);
    m_isRestoringBoxState = false;
    m_gridService.storeSelectedBox(boxId);
    m_gridControl.sendViewStateToGrid();
}

// ================================================================
// SLICE: hornet command system - save
// ================================================================

QString Control::saveProjectToFile(const std::filesystem::path &filePath)
{
    std::ofstream outStream(filePath);
    if (!outStream.is_open())
        return "could not open file for writing: " + QString::fromStdString(filePath.string());

    const GridSaveDataDTO saveData = m_gridService.retrieveGridSaveData();
    const std::filesystem::path saveDir = filePath.parent_path();

    int savedCount = 0;
    int skippedCount = 0;
    for (const BoxSaveDataDTO &box : saveData.boxes) {
        if (!box.isFileBacked) {
            ++skippedCount;
            continue;
        }
        const std::filesystem::path originPath(box.originFilePath.toStdString());
        std::filesystem::path relativePath;
        try {
            relativePath = std::filesystem::relative(originPath, saveDir);
        } catch (const std::filesystem::filesystem_error &) {
            relativePath = originPath;
        }

        outStream << "hornet load " << relativePath.string() << "\n";
        outStream << "hornet setpos last " << box.posX << " " << box.posY << "\n";
        outStream << "hornet setsize last " << box.width << " " << box.height << "\n";
        outStream << "hornet setscroll last " << box.scrollOffset << "\n";
        outStream << "hornet setcursor last " << box.cursorX << " " << box.cursorY << "\n\n";
        ++savedCount;
    }

    outStream << "hornet setzoom " << saveData.zoomLevel << "\n";
    outStream << "hornet setoffset " << saveData.offset.x() << " " << saveData.offset.y() << "\n";
    outStream.close();

    QString message = "saved " + QString::number(savedCount) + " box(es) to "
                      + QString::fromStdString(filePath.string());
    if (skippedCount > 0)
        message += " (skipped " + QString::number(skippedCount) + " non-file-backed box(es))";
    return message;
}

// ================================================================
// SLICE: box unload (close button - separate from "hornet unload")
// ================================================================

void Control::onBoxUnloadRequested(const BoxUnloadRequestedDTO &dto)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    const HornetCommandDTO command{true, "unload", QString::number(dto.boxId), workingDir};
    const QString message = dispatchHornetCommand(command);
    if (!message.isEmpty()) {
        appendToLogBox("unload " + QString::number(dto.boxId), message);
        m_gridControl.sendViewStateToGrid();
    }
}

// ================================================================
// SLICE: tool button activation (Ctrl+click on a .tool box's button)
// ================================================================

void Control::onToolButtonActivated(const ToolButtonActivatedDTO &dto)
{
    if (!m_toolControl.isCommandTrusted(dto.hornetCommand)) {
        m_pendingTrustButtonBoxId = dto.boxId;
        m_pendingTrustButtonCommand = dto.hornetCommand;

        const std::vector<QString> declaredButtonCommands = m_gridService.retrieveToolButtonCommands(
            dto.boxId);
        QStringList untrustedButtons;
        for (const QString &command : declaredButtonCommands)
            if (!m_toolControl.isCommandTrusted(command))
                untrustedButtons.push_back(command);

        const std::vector<ToolSourceDTO> declaredSources = m_gridService.retrieveToolSources(
            dto.boxId);
        QStringList untrustedSources;
        for (const ToolSourceDTO &source : declaredSources)
            if (!m_toolControl.isCommandTrusted(source.command))
                untrustedSources.push_back(source.command);

        const std::vector<ToolListSourceDTO> declaredListSources
            = m_gridService.retrieveToolListSources(dto.boxId);
        for (const ToolListSourceDTO &listSource : declaredListSources)
            if (!m_toolControl.isCommandTrusted(listSource.command))
                untrustedSources.push_back(listSource.command);

        m_windowControl.sendToolTrustPromptToWindow(
            ToolTrustPromptDTO(dto.boxId, untrustedButtons, untrustedSources));
        return;
    }
    dispatchToolButtonCommand(dto.boxId, dto.hornetCommand);
}

void Control::dispatchToolButtonCommand(int boxId, const QString &literalCommand)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    const QHash<QString, QString> fieldValues = m_gridService.retrieveToolFieldValues(boxId);
    const QString substitutedCommand = ToolScriptParser::substituteValues(literalCommand,
                                                                          fieldValues);
    const HornetCommandDTO hornetCommand
        = m_terminalControl.parseHornetCommand("hornet " + substitutedCommand, workingDir);
    if (!hornetCommand.wasHornetCommand)
        return;
    const QString message = dispatchHornetCommand(hornetCommand);
    if (!message.isEmpty()) {
        appendToLogBox(substitutedCommand, message);
        m_gridControl.sendViewStateToGrid();
    }
}

void Control::onToolTrustAllRequested(const BoxUnloadRequestedDTO &dto)
{
    QStringList toTrust;

    const std::vector<QString> declaredButtonCommands = m_gridService.retrieveToolButtonCommands(
        dto.boxId);
    for (const QString &command : declaredButtonCommands)
        if (!m_toolControl.isCommandTrusted(command))
            toTrust.push_back(command);

    const std::vector<ToolSourceDTO> declaredSources = m_gridService.retrieveToolSources(dto.boxId);
    for (const ToolSourceDTO &source : declaredSources)
        if (!m_toolControl.isCommandTrusted(source.command))
            toTrust.push_back(source.command);

    const std::vector<ToolListSourceDTO> declaredListSources = m_gridService.retrieveToolListSources(
        dto.boxId);
    for (const ToolListSourceDTO &listSource : declaredListSources)
        if (!m_toolControl.isCommandTrusted(listSource.command))
            toTrust.push_back(listSource.command);

    m_toolControl.trustCommands(toTrust);

    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    m_toolControl.dispatchToolCommand(dto.boxId, workingDir);

    if (m_pendingTrustButtonBoxId == dto.boxId && !m_pendingTrustButtonCommand.isEmpty()) {
        const QString commandToRun = m_pendingTrustButtonCommand;
        m_pendingTrustButtonBoxId = -1;
        m_pendingTrustButtonCommand.clear();
        dispatchToolButtonCommand(dto.boxId, commandToRun);
    }
}

void Control::onToolTextFieldCommitted(const ToolTextFieldCommitDTO &dto)
{
    m_gridService.storeToolFieldValue(dto.boxId, dto.fieldName, dto.value);
    m_gridControl.sendViewStateToGrid();
}

// ================================================================
// SLICE: box list paging (titlebar dropdown, file loader popup)
// ================================================================

void Control::onBoxListPageRequested(const BoxListPageRequestDTO &dto)
{
    const int totalCount = m_gridService.retrieveBoxCount();
    const int highestBoxId = m_gridService.retrieveHighestBoxId();
    const std::vector<BoxListEntryDTO> entries
        = m_gridService.retrieveBoxHeaderListPage(dto.startIndex, dto.count);
    m_windowControl.sendBoxListPageToTitlebar(
        BoxListPageDTO{dto.startIndex, totalCount, highestBoxId, entries});
}

void Control::onPopupBoxListPageRequested(const BoxListPageRequestDTO &dto)
{
    const int totalCount = m_gridService.retrieveBoxCount();
    const int highestBoxId = m_gridService.retrieveHighestBoxId();
    const std::vector<BoxListEntryDTO> entries
        = m_gridService.retrieveBoxHeaderListPage(dto.startIndex, dto.count);
    m_windowControl.sendBoxListPageToPopup(
        BoxListPageDTO{dto.startIndex, totalCount, highestBoxId, entries});
}

void Control::onFileLoaderLoadRequested(const FilePathListDTO &dto)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    for (const QString &filePath : dto.filePaths) {
        const HornetCommandDTO command{true, "load", filePath, workingDir};
        const QString message = dispatchHornetCommand(command);
        if (!message.isEmpty())
            appendToLogBox("load " + filePath, message);
    }
}

void Control::onProjectSaverSaveRequested(const QString &baseName)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    const QString fileName = baseName + ".script";
    const HornetCommandDTO command{true, "save", fileName, workingDir};
    const QString message = dispatchHornetCommand(command);
    if (!message.isEmpty())
        appendToLogBox("save " + fileName, message);
    m_windowControl.sendProjectSaveResultToPopup(message);
}

// ================================================================
// SLICE: script runner (box-list run, browse-and-run)
// ================================================================

void Control::onScriptRunnerBoxRunRequested(int boxId)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    QString originFilePath;
    try {
        originFilePath = m_gridService.retrieveBoxOriginFilePath(boxId);
    } catch (const std::runtime_error &) {
        appendToLogBox("run #" + QString::number(boxId), "no box with id " + QString::number(boxId));
        return;
    }
    if (originFilePath.isEmpty()) {
        appendToLogBox("run #" + QString::number(boxId), "box is not file-backed");
        return;
    }
    const HornetCommandDTO command{true, "run", originFilePath, workingDir};
    const QString message = dispatchHornetCommand(command);
    if (!message.isEmpty())
        appendToLogBox("run " + originFilePath, message);
}

void Control::onScriptRunnerRunRequested(const FilePathListDTO &dto)
{
    const std::filesystem::path workingDir = m_terminalService.retrieveCurrentDirectory();
    for (const QString &filePath : dto.filePaths) {
        const HornetCommandDTO command{true, "run", filePath, workingDir};
        const QString message = dispatchHornetCommand(command);
        if (!message.isEmpty())
            appendToLogBox("run " + filePath, message);
    }
}

// ================================================================
// SLICE: debug
// ================================================================

void Control::onDebugRequested()
{
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const
{
    const WindowModel &windowModel = m_modelAccess.getWindowModel();
    const EditorModel &editorModel = m_modelAccess.getEditorModel();
    const GridModel &gridModel = m_modelAccess.getGridModel();
    qDebug() << "===" << " MODEL STATE" << debugPrintCounter << "===";
    qDebug() << "WindowModel:";
    qDebug() << "    " << "x:" << windowModel.getX() << "y:" << windowModel.getY()
             << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight()
             << "fullscreen:" << windowModel.isFullscreen();

    qDebug() << "EditorModel:";
    qDebug() << "    " << "noOfVisibleLines:" << editorModel.getNoOfVisibleLines()
             << "topLineIndex:" << editorModel.getTopLineIndex();
    qDebug() << "    " << "noOfLines:" << editorModel.getNoOfLines()
             << "noOfCharsOfLongestLine:" << editorModel.getNoOfCharsOfLongestLine()
             << "fileType:" << editorModel.getFileType();
    qDebug() << "    " << "cursorX:" << editorModel.getCursorX()
             << "cursorY:" << editorModel.getCursorY();

    qDebug() << "GridModel:";
    qDebug() << "    " << "zoomLevel:" << gridModel.getZoomLevel()
             << "gridGap:" << gridModel.getGridGap() << "offset:" << gridModel.getOffset();

    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
