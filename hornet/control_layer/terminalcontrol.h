#pragma once
#include <QObject>
#include <QProcess>
#include <filesystem>
class IModelAccessRead;
class EditorService;
class TerminalService;
struct EditorKeyPressDTO;
struct EditorCursorPosDTO;

struct HornetCommandDTO
{
    bool wasHornetCommand;
    QString subcommand;
    QString argument;
    std::filesystem::path workingDirectory;
};

struct CommandExecutionResultDTO
{
    QString commandText;
    HornetCommandDTO hornetCommand;
    QString shellOutput;
};

struct TerminalKeyPressResultDTO
{
    bool wasHandled;
    CommandExecutionResultDTO commandExecutionResult;
};

class TerminalControl : public QObject
{
    Q_OBJECT
public:
    explicit TerminalControl(IModelAccessRead &modelAccess,
                             EditorService &editorService,
                             TerminalService &terminalService);
    void init();
    TerminalKeyPressResultDTO dispatchTerminalKeyPress(const EditorKeyPressDTO &dto);
    void removePromptForDeletedLine(int lineCountBefore, int cursorYBefore);
    void dispatchEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    CommandExecutionResultDTO executeCommand();
    HornetCommandDTO checkForHornetCommand(int cursorY);
    HornetCommandDTO parseHornetCommand(const QString &line,
                                        const std::filesystem::path &workingDir) const;
    QString runShellCommandInDirectory(const QString &command,
                                       const std::filesystem::path &workingDir,
                                       std::filesystem::path &outResultingDirectory);

private:
    QString getCurrentLineAsQString(int cursorY) const;
    std::filesystem::path getWorkingDirForLine(int cursorY) const;
    QString runCommand(const QString &command, const std::filesystem::path &workingDir);
    void handleLastLineExecution();
    void handleNonLastLineExecution(int cursorY, int lastLineNumber);
    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    TerminalService &m_terminalService;
    QProcess m_process;
};
