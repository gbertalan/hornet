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

#include <fstream>
#include <iostream>

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
{}

void Control::init()
{
    m_windowControl.init();
    m_editorControl.init();
    m_terminalControl.init();
    m_gridControl.init();

    const int terminalBoxId = m_gridService.findFirstBoxIdOfType(BoxContentType::Terminal);
    if (terminalBoxId != -1) {
        m_editorService.setIsTerminal(true);
        m_editorControl.sendStateToEditor(buildTerminalPrompts());
        m_editorControl.sendSettingsToEditor();
        m_currentlySelectedBoxId = terminalBoxId;
        m_gridService.setSelectedBox(terminalBoxId);
        m_gridControl.refreshGridViewState();
    }
}
void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowControl.dispatchWindowStateChanged(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});

    if (m_currentlySelectedBoxId != -1 && dto.topLineIndex != m_lastSyncedBoxScrollOffset) {
        m_gridService.setBoxScrollOffset(m_currentlySelectedBoxId, dto.topLineIndex);
        m_lastSyncedBoxScrollOffset = dto.topLineIndex;
        m_gridControl.refreshGridViewState();
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
        m_gridControl.refreshGridViewState();
    }
}

void Control::onEditorKeyPressed(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::CtrlD) {
        onDebugRequested();
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
                        createCommandOutputBox(executionResult.commandText, hornetMessage);
                } else {
                    createCommandOutputBox(executionResult.commandText, executionResult.shellOutput);
                }
                m_gridControl.refreshGridViewState();
            }
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
        m_gridControl.refreshGridViewState();
    }
    m_editorControl.sendStateToEditor(
        m_modelAccess.getEditorModel().isTerminal() ? buildTerminalPrompts() : QVector<QString>{});
    m_editorControl.sendCursorPosToEditor();
}

void Control::onGridZoomChanged(const GridZoomDTO &dto)
{
    m_gridControl.dispatchGridZoomChange(dto);
}

void Control::onGridDrag(const GridDragDTO &dto)
{
    m_gridControl.dispatchGridDrag(dto);
}

void Control::onBoxDragged(const BoxDragDTO &dto)
{
    m_gridControl.dispatchBoxDrag(dto);
}

QVector<QString> Control::buildTerminalPrompts() const
{
    QVector<QString> terminalPrompts;
    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_modelAccess.getTerminalModel().getTerminalPromptAndDirs();
    for (const TerminalPromptAndDir &line : terminalPromptAndDirs)
        terminalPrompts.push_back(
            QString::fromUcs4(reinterpret_cast<const char32_t *>(line.prompt.c_str()),
                              static_cast<int>(line.prompt.size())));
    return terminalPrompts;
}

void Control::onBoxSelected(const BoxSelectedDTO &dto)
{
    const int previouslySelectedBoxId = m_currentlySelectedBoxId;
    if (previouslySelectedBoxId != -1)
        flushEditorContentToBox(previouslySelectedBoxId);

    m_currentlySelectedBoxId = dto.boxId;
    m_lastSyncedBoxScrollOffset = -1;

    m_isRestoringBoxState = true;

    const BoxContentDTO boxContent = m_gridService.retrieveBoxContent(dto.boxId);
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
    m_editorControl.sendSettingsToEditor();

    m_isRestoringBoxState = false;

    m_gridService.setSelectedBox(dto.boxId);
    m_gridControl.refreshGridViewState();
}

void Control::onBoxResized(const BoxResizeDTO &dto)
{
    m_gridControl.dispatchBoxResize(dto);
}

void Control::flushEditorContentToBox(int boxId)
{
    const std::vector<std::u32string> &currentLines = m_modelAccess.getEditorModel().getTextLines();
    QVector<QString> linesAsQString;
    linesAsQString.reserve(static_cast<int>(currentLines.size()));
    for (const std::u32string &line : currentLines)
        linesAsQString.push_back(convertU32StringToQString(line));

    const int cursorX = m_modelAccess.getEditorModel().getCursorX();
    const int cursorY = m_modelAccess.getEditorModel().getCursorY();
    m_gridService.updateBoxContent(boxId, linesAsQString, cursorX, cursorY);
}

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
    m_gridService.addBox(0, 0, 20, 15, headerText, bodyLines);
    return true;
}

QString Control::dispatchHornetCommand(const HornetCommandDTO &command)
{
    if (command.subcommand == "load") {
        const std::filesystem::path filePath = command.workingDirectory
                                               / command.argument.toStdString();
        if (loadFileIntoNewBox(filePath)) {
            m_gridControl.refreshGridViewState();
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

        m_gridControl.refreshGridViewState();
        return "loaded " + QString::number(filesLoaded) + " file(s)";
    }

    if (command.subcommand == "setpos") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setpos <boxId> <x> <y>";

        bool boxIdOk = false;
        bool xOk = false;
        bool yOk = false;
        const int boxId = parts.at(0).toInt(&boxIdOk);
        const int x = parts.at(1).toInt(&xOk);
        const int y = parts.at(2).toInt(&yOk);
        if (!boxIdOk || !xOk || !yOk)
            return "usage: hornet setpos <boxId> <x> <y> (all must be integers)";

        try {
            m_gridService.setBoxPosition(boxId, x, y);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }

        m_gridControl.refreshGridViewState();
        return "";
    }

    if (command.subcommand == "setsize") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setsize <boxId> <width> <height>";
        bool boxIdOk = false, widthOk = false, heightOk = false;
        const int boxId = parts.at(0).toInt(&boxIdOk);
        const int width = parts.at(1).toInt(&widthOk);
        const int height = parts.at(2).toInt(&heightOk);
        if (!boxIdOk || !widthOk || !heightOk)
            return "usage: hornet setsize <boxId> <width> <height> (all must be integers)";
        try {
            m_gridService.setBoxSize(boxId, width, height);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.refreshGridViewState();
        return "";
    }

    if (command.subcommand == "setscroll") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 2)
            return "usage: hornet setscroll <boxId> <offset>";
        bool boxIdOk = false, offsetOk = false;
        const int boxId = parts.at(0).toInt(&boxIdOk);
        const int offset = parts.at(1).toInt(&offsetOk);
        if (!boxIdOk || !offsetOk)
            return "usage: hornet setscroll <boxId> <offset> (all must be integers)";
        try {
            m_gridService.setBoxScrollOffset(boxId, offset);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.refreshGridViewState();
        return "";
    }

    if (command.subcommand == "setcursor") {
        const QStringList parts = command.argument.split(' ', Qt::SkipEmptyParts);
        if (parts.size() < 3)
            return "usage: hornet setcursor <boxId> <x> <y>";
        bool boxIdOk = false, xOk = false, yOk = false;
        const int boxId = parts.at(0).toInt(&boxIdOk);
        const int x = parts.at(1).toInt(&xOk);
        const int y = parts.at(2).toInt(&yOk);
        if (!boxIdOk || !xOk || !yOk)
            return "usage: hornet setcursor <boxId> <x> <y> (all must be integers)";
        try {
            m_gridService.setCursorPosition(boxId, x, y);
        } catch (const std::runtime_error &) {
            return "no box with id " + QString::number(boxId);
        }
        m_gridControl.refreshGridViewState();
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
        m_gridService.setZoomLevel(zoomLevel);
        m_gridControl.refreshGridViewState();
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
        m_gridService.setGridOffset(x, y);
        m_gridControl.refreshGridViewState();
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
                bool ok = false;
                const int id = part.toInt(&ok);
                if (!ok)
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
        for (const int id : idsToRemove) {
            try {
                m_gridService.removeBox(id);
            } catch (const std::runtime_error &) {
                notFound.push_back(QString::number(id));
            }
        }

        m_gridControl.refreshGridViewState();
        if (!notFound.isEmpty())
            return "no box(es) with id: " + notFound.join(", ");
        return "";
    }

    return "unknown hornet command: " + command.subcommand;
}

void Control::createCommandOutputBox(const QString &commandText, const QString &outputText)
{
    QVector<QString> bodyLines;
    if (outputText.isEmpty()) {
        bodyLines.push_back("(no output)");
    } else {
        for (const QString &line : outputText.split('\n'))
            bodyLines.push_back(line);
    }
    m_gridService.addBox(0, 0, 20, 15, commandText, bodyLines);
}

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
