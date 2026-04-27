#pragma once
#include <QObject>
#include <QProcess>
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
    bool handleTerminalKeyPress(const EditorKeyPressDTO &dto);
    void onEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void executeCommand();

private:
    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    TerminalService &m_terminalService;
    QProcess m_process;
};
