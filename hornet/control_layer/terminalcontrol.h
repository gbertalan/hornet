#pragma once
#include <QObject>
#include <QProcess>
#include <filesystem>
class IModelAccessRead;
class EditorService;
class TerminalService;
struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
class TerminalControl : public QObject
{
    Q_OBJECT
public:
    explicit TerminalControl(IModelAccessRead &modelAccess,
                             EditorService &editorService,
                             TerminalService &terminalService);
    void init();
    bool dispatchTerminalKeyPress(const EditorKeyPressDTO &dto);
    void postKeyPress(int lineCountBefore, int cursorYBefore);
    void dispatchEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void executeCommand();

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
