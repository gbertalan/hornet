#include "control.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/terminalmodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/terminalservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"
#include <iostream>
#include <shared/dto_model_to_view/editorviewstatedto.h>

Control::Control(IModelAccessRead &modelAccess,
                 WindowService &windowService,
                 EditorService &editorService,
                 TerminalService &terminalService,
                 View &view)
    : m_modelAccess(modelAccess)
    , m_windowService(windowService)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
    , m_view(view)
{}

void Control::init()
{

    m_editorService.setTextLines({U""}, "txt"); // never empty

    m_isTerminal = true;
    if (m_isTerminal) {
        // int lastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
        // EditorCursorPosDTO dto{0, lastLine};
        // m_editorService.storeCursorPos(dto);
        // sendCursorPosToEditor();
        m_terminalService.initialize();
    }
}

void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowService.storeWindowState(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
    sendStateToEditor();
}

/**
 * @brief Control::sendStateToEditor Retrieves visible lines (and metadata) from Model and sends
 * them to View
 */
void Control::sendStateToEditor()
{
    std::vector<std::u32string> lines = m_editorService.retrieveActiveLines();
    if (lines.empty())
        return;
    int noOfAllLines = m_modelAccess.getEditorModel().getNoOfLines();
    int noOfCharsOfLongestLine = m_modelAccess.getEditorModel().getNoOfCharsOfLongestLine();
    std::string fileType = m_modelAccess.getEditorModel().getFileType();
    QVector<QString> qLines;
    qLines.reserve(static_cast<int>(lines.size()));
    for (const std::u32string &line : lines)
        qLines.push_back(QString::fromUcs4(reinterpret_cast<const char32_t *>(line.c_str()),
                                           static_cast<int>(line.size())));
    QVector<QString> terminalPrompts;
    if (m_isTerminal) {
        const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
            = m_terminalService.getTerminalPromptAndDirs();
        for (const TerminalPromptAndDir &line : terminalPromptAndDirs)
            terminalPrompts.push_back(
                QString::fromUcs4(reinterpret_cast<const char32_t *>(line.prompt.c_str()),
                                  static_cast<int>(line.prompt.size())));
    }
    EditorViewStateDTO dto{qLines,
                           noOfAllLines,
                           noOfCharsOfLongestLine,
                           QString::fromStdString(fileType),
                           terminalPrompts};
    m_view.updateEditorState(dto);
}

void Control::onEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    m_editorService.storeCursorPos(dto);
    if (m_isTerminal) {
        const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
            = m_terminalService.getTerminalPromptAndDirs();
        if (dto.cursorY < static_cast<int>(terminalPromptAndDirs.size()))
            m_terminalService.setCurrentDirectory(terminalPromptAndDirs.at(dto.cursorY).directory);
    }
    sendCursorPosToEditor();
}

void Control::sendCursorPosToEditor()
{
    int cursorX = m_modelAccess.getEditorModel().getCursorX();
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    EditorCursorPosDTO dtoToSendToView{cursorX, cursorY};
    m_view.updateEditorCursorPos(dtoToSendToView);
}

void Control::onEditorKeyPressed(const EditorKeyPressDTO &dto)
{
    if (m_isTerminal && handleTerminalKeyPress(dto))
        return;
    handleEditorKeyPress(dto);
    sendStateToEditor();
    sendCursorPosToEditor();
}

bool Control::handleTerminalKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter) {
        executeCommand();
        sendStateToEditor();
        return true;
    }
    return false;
}

void Control::handleEditorKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.alt)
        return;
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::None) {
        if (dto.ctrl)
            return;
        m_editorService.insertCharacter(dto.key);
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace
               || dto.specialKey == EditorKeyPressDTO::SpecialKey::Delete) {
        if (dto.ctrl) {
            if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Backspace)
                m_editorService.deleteWordLeft();
            else
                m_editorService.deleteWordRight();
        } else {
            m_editorService.deleteCharacter(dto);
        }
    } else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter)
        m_editorService.insertNewLine();
    else if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Tab)
        m_editorService.insertTab();
    else
        m_editorService.moveCursor(dto);
}

void Control::executeCommand()
{
    const std::vector<std::u32string> &lines = m_modelAccess.getEditorModel().getTextLines();
    if (lines.empty())
        return;

    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    int lastLineNumber = m_modelAccess.getEditorModel().getNoOfLines() - 1;
    const std::u32string &currentLine = lines.at(cursorY);
    if (currentLine.empty())
        return;

    QString command = QString::fromUcs4(reinterpret_cast<const char32_t *>(currentLine.c_str()),
                                        static_cast<int>(currentLine.size()));

    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_terminalService.getTerminalPromptAndDirs();

    std::filesystem::path workingDir = m_terminalService.getCurrentDirectory();
    if (cursorY < static_cast<int>(terminalPromptAndDirs.size()))
        workingDir = terminalPromptAndDirs.at(cursorY).directory;

    QString sentinel = "---HORNET_PWD---";
    QString combinedCommand = command + "; echo " + sentinel + "; pwd";
    if (cursorY == lastLineNumber)
        m_terminalService.setCurrentDirectory(workingDir);
    m_process.setWorkingDirectory(QString::fromStdString(workingDir.string()));

    m_process.start("/bin/sh", QStringList() << "-c" << combinedCommand);
    m_process.waitForFinished();
    QString output = QString::fromUtf8(m_process.readAllStandardOutput());
    int sentinelIndex = output.indexOf(sentinel);
    if (sentinelIndex != -1) {
        QString commandOutput = output.left(sentinelIndex).trimmed();
        QString newDir = output.mid(sentinelIndex + sentinel.length()).trimmed();
        if (!commandOutput.isEmpty())
            std::cout << commandOutput.toStdString() << std::endl;
        m_terminalService.setCurrentDirectory(std::filesystem::path(newDir.toStdString()));
    } else {
        std::cout << output.toStdString() << std::endl;
    }

    if (cursorY == lastLineNumber) {
        std::filesystem::path newLineDir = m_terminalService.getCurrentDirectory();
        std::u32string newLinePrompt = m_terminalService.getCurrentPrompt();
        m_terminalService.addTerminalPromptAndDir({newLinePrompt, newLineDir});
        std::vector<std::u32string> updatedLines = m_modelAccess.getEditorModel().getTextLines();
        updatedLines.push_back(U"");
        m_editorService.setTextLines(updatedLines, "txt");
        int newLastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
        EditorCursorPosDTO cursorDto{0, newLastLine};
        m_editorService.storeCursorPos(cursorDto);
        m_terminalService.setCurrentDirectory(newLineDir);
    } else { // not last line
        m_terminalService.updateTerminalLineDirectory(cursorY,
                                                      m_terminalService.getCurrentDirectory());
        const std::vector<TerminalPromptAndDir> &updatedTerminalPromptAndDirs
            = m_terminalService.getTerminalPromptAndDirs();
        if (lastLineNumber < static_cast<int>(updatedTerminalPromptAndDirs.size()))
            m_terminalService.setCurrentDirectory(
                updatedTerminalPromptAndDirs.at(lastLineNumber).directory);
    }
    sendStateToEditor();
    sendCursorPosToEditor();
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
    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
