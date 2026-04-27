#include "terminalcontrol.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/terminalmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/terminalservice.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include <iostream>

TerminalControl::TerminalControl(IModelAccessRead &modelAccess,
                                 EditorService &editorService,
                                 TerminalService &terminalService)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
{}

void TerminalControl::onEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_terminalService.getTerminalPromptAndDirs();
    if (dto.cursorY < static_cast<int>(terminalPromptAndDirs.size()))
        m_terminalService.setCurrentDirectory(terminalPromptAndDirs.at(dto.cursorY).directory);
}

bool TerminalControl::handleTerminalKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter) {
        executeCommand();
        return true;
    }
    return false;
}

void TerminalControl::executeCommand()
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
    } else {
        m_terminalService.updateTerminalLineDirectory(cursorY,
                                                      m_terminalService.getCurrentDirectory());
        const std::vector<TerminalPromptAndDir> &updatedTerminalPromptAndDirs
            = m_terminalService.getTerminalPromptAndDirs();
        if (lastLineNumber < static_cast<int>(updatedTerminalPromptAndDirs.size()))
            m_terminalService.setCurrentDirectory(
                updatedTerminalPromptAndDirs.at(lastLineNumber).directory);
    }
}
