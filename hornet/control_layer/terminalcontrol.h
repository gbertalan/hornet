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

    // ================================================================
    // SLICE: live terminal key/command dispatch (typed into the terminal box)
    // ================================================================
    TerminalKeyPressResultDTO dispatchTerminalKeyPress(const EditorKeyPressDTO &dto);
    CommandExecutionResultDTO executeCommand();
    void dispatchEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void removePromptForDeletedLine(int lineCountBefore, int cursorYBefore);

    // ================================================================
    // SLICE: hornet command parsing (shared by live terminal AND by
    // Control's script execution - see parseHornetCommand)
    // ================================================================
    HornetCommandDTO checkForHornetCommand(int cursorY);
    HornetCommandDTO parseHornetCommand(const QString &line,
                                        const std::filesystem::path &workingDir) const;

    // ================================================================
    // SLICE: shell execution for scripts (local QProcess, does NOT
    // touch the live terminal's own process/current-directory state)
    // ================================================================
    QString runShellCommandInDirectory(const QString &command,
                                       const std::filesystem::path &workingDir,
                                       std::filesystem::path &outResultingDirectory);

private:
    // ================================================================
    // SLICE: command line reading / working-directory helpers
    // ================================================================
    QString getCurrentLineAsQString(int cursorY) const;
    std::filesystem::path getWorkingDirForLine(int cursorY) const;

    // ================================================================
    // SLICE: live terminal shell execution + post-execution bookkeeping
    // ================================================================
    QString runCommand(const QString &command, const std::filesystem::path &workingDir);
    void handleLastLineExecution();
    void handleNonLastLineExecution(int cursorY, int lastLineNumber);

    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    TerminalService &m_terminalService;
    QProcess m_process;
};
