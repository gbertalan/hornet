#include "terminalcontrol.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/terminalmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/terminalservice.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include <iostream>
#include <qdebug.h>

TerminalControl::TerminalControl(IModelAccessRead &modelAccess,
                                 EditorService &editorService,
                                 TerminalService &terminalService)
    : m_modelAccess(modelAccess)
    , m_editorService(editorService)
    , m_terminalService(terminalService)
{}

void TerminalControl::init()
{
    m_terminalService.init();
}

void TerminalControl::dispatchEditorCursorPosChanged(const EditorCursorPosDTO &dto)
{
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    const std::vector<TerminalPromptAndDir> &terminalPromptAndDirs
        = m_terminalService.getTerminalPromptAndDirs();
    if (cursorY < static_cast<int>(terminalPromptAndDirs.size()))
        m_terminalService.setCurrentDirectory(terminalPromptAndDirs.at(cursorY).directory);
}

TerminalKeyPressResultDTO TerminalControl::dispatchTerminalKeyPress(const EditorKeyPressDTO &dto)
{
    if (dto.specialKey == EditorKeyPressDTO::SpecialKey::Enter) {
        const CommandExecutionResultDTO result = executeCommand();
        return TerminalKeyPressResultDTO{true, result};
    }
    const CommandExecutionResultDTO
        emptyResult{"",
                    HornetCommandDTO{false, "", "", m_terminalService.getCurrentDirectory()},
                    ""};
    return TerminalKeyPressResultDTO{false, emptyResult};
}

CommandExecutionResultDTO TerminalControl::executeCommand()
{
    const std::vector<std::u32string> &lines = m_modelAccess.getEditorModel().getTextLines();
    const CommandExecutionResultDTO
        emptyResult{"",
                    HornetCommandDTO{false, "", "", m_terminalService.getCurrentDirectory()},
                    ""};
    if (lines.empty())
        return emptyResult;
    int cursorY = m_modelAccess.getEditorModel().getCursorY();
    int lastLineNumber = m_modelAccess.getEditorModel().getNoOfLines() - 1;
    if (lines.at(cursorY).empty())
        return emptyResult;

    const QString commandText = getCurrentLineAsQString(cursorY);
    const HornetCommandDTO hornetCommand = checkForHornetCommand(cursorY);

    QString shellOutput;
    if (!hornetCommand.wasHornetCommand) {
        std::filesystem::path workingDir = getWorkingDirForLine(cursorY);
        if (cursorY == lastLineNumber)
            m_terminalService.setCurrentDirectory(workingDir);
        shellOutput = runCommand(commandText, workingDir);
    } else {
        m_terminalService.setCurrentDirectory(hornetCommand.workingDirectory);
    }

    if (cursorY == lastLineNumber)
        handleLastLineExecution();
    else
        handleNonLastLineExecution(cursorY, lastLineNumber);

    return CommandExecutionResultDTO{commandText, hornetCommand, shellOutput};
}

QString TerminalControl::getCurrentLineAsQString(int cursorY) const
{
    const std::vector<std::u32string> &lines = m_modelAccess.getEditorModel().getTextLines();
    const std::u32string &line = lines.at(cursorY);
    return QString::fromUcs4(reinterpret_cast<const char32_t *>(line.c_str()),
                             static_cast<int>(line.size()));
}

std::filesystem::path TerminalControl::getWorkingDirForLine(int cursorY) const
{
    const std::vector<TerminalPromptAndDir> &promptAndDirs = m_terminalService
                                                                 .getTerminalPromptAndDirs();
    if (cursorY < static_cast<int>(promptAndDirs.size()))
        return promptAndDirs.at(cursorY).directory;

    return m_terminalService.getCurrentDirectory();
}

QString TerminalControl::runCommand(const QString &command, const std::filesystem::path &workingDir)
{
    QString sentinel = "---SENTINEL---";
    QString combinedCommand = command + "; echo " + sentinel + "; pwd";
    m_process.setWorkingDirectory(QString::fromStdString(workingDir.string()));
    m_process.start("/bin/sh", QStringList() << "-c" << combinedCommand);
    m_process.waitForFinished();
    QString output = QString::fromUtf8(m_process.readAllStandardOutput());
    int sentinelIndex = output.indexOf(sentinel);
    QString commandOutput;
    if (sentinelIndex != -1) {
        commandOutput = output.left(sentinelIndex).trimmed();
        QString newDir = output.mid(sentinelIndex + sentinel.length()).trimmed();
        if (!commandOutput.isEmpty())
            std::cout << commandOutput.toStdString() << std::endl;
        m_terminalService.setCurrentDirectory(std::filesystem::path(newDir.toStdString()));
    } else {
        commandOutput = output.trimmed();
        std::cout << output.toStdString() << std::endl;
    }
    return commandOutput;
}

void TerminalControl::handleLastLineExecution()
{
    std::filesystem::path newLineDir = m_terminalService.getCurrentDirectory();
    std::u32string newLinePrompt = m_terminalService.getCurrentPrompt();
    m_terminalService.addTerminalPromptAndDir({newLinePrompt, newLineDir});
    std::vector<std::u32string> updatedLines = m_modelAccess.getEditorModel().getTextLines();
    updatedLines.push_back(U"");
    m_editorService.storeTextLines(updatedLines, "txt");
    int newLastLine = m_modelAccess.getEditorModel().getNoOfLines() - 1;
    EditorCursorPosDTO cursorDto{0, newLastLine};
    m_editorService.storeCursorPos(cursorDto);
    m_terminalService.setCurrentDirectory(newLineDir);
}

void TerminalControl::handleNonLastLineExecution(int cursorY, int lastLineNumber)
{
    m_terminalService.updateTerminalLineDirectory(cursorY, m_terminalService.getCurrentDirectory());
    const std::vector<TerminalPromptAndDir> &updatedPromptAndDirs = m_terminalService
                                                                        .getTerminalPromptAndDirs();
    if (lastLineNumber < static_cast<int>(updatedPromptAndDirs.size()))
        m_terminalService.setCurrentDirectory(updatedPromptAndDirs.at(lastLineNumber).directory);
}

void TerminalControl::removePromptForDeletedLine(int lineCountBefore, int cursorYBefore)
{
    int lineCountAfter = m_modelAccess.getEditorModel().getNoOfLines();
    if (lineCountAfter < lineCountBefore)
        m_terminalService.removeTerminalPromptAndDir(cursorYBefore);
}

HornetCommandDTO TerminalControl::checkForHornetCommand(int cursorY)
{
    const QString line = getCurrentLineAsQString(cursorY);
    const std::filesystem::path workingDir = getWorkingDirForLine(cursorY);
    return parseHornetCommand(line, workingDir);
}

HornetCommandDTO TerminalControl::parseHornetCommand(const QString &line,
                                                     const std::filesystem::path &workingDir) const
{
    const QString trimmedLine = line.trimmed();
    const QString prefix = "hornet ";

    if (!trimmedLine.startsWith(prefix))
        return HornetCommandDTO{false, "", "", workingDir};

    const QString remainder = trimmedLine.mid(prefix.length()).trimmed();
    const int spaceIndex = remainder.indexOf(' ');
    if (spaceIndex == -1)
        return HornetCommandDTO{true, remainder, "", workingDir};

    const QString subcommand = remainder.left(spaceIndex);
    const QString argument = remainder.mid(spaceIndex + 1).trimmed();
    return HornetCommandDTO{true, subcommand, argument, workingDir};
}

QString TerminalControl::runShellCommandInDirectory(const QString &command,
                                                    const std::filesystem::path &workingDir,
                                                    std::filesystem::path &outResultingDirectory)
{
    QString sentinel = "---SENTINEL---";
    QString combinedCommand = command + "; echo " + sentinel + "; pwd";
    QProcess process;
    process.setWorkingDirectory(QString::fromStdString(workingDir.string()));
    process.start("/bin/sh", QStringList() << "-c" << combinedCommand);
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    int sentinelIndex = output.indexOf(sentinel);
    QString commandOutput;
    if (sentinelIndex != -1) {
        commandOutput = output.left(sentinelIndex).trimmed();
        QString newDir = output.mid(sentinelIndex + sentinel.length()).trimmed();
        outResultingDirectory = std::filesystem::path(newDir.toStdString());
    } else {
        commandOutput = output.trimmed();
        outResultingDirectory = workingDir;
    }
    return commandOutput;
}
