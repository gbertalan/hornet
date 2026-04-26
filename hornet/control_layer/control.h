#pragma once
#include <QDebug>
#include <QObject>
#include <QProcess>
struct EditorKeyPressDTO;
class IModelAccessRead;
class WindowService;
class EditorService;
class TerminalService;
class View;
struct WindowDTO;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(IModelAccessRead &modelAccess,
                     WindowService &windowService,
                     EditorService &editorService,
                     TerminalService &terminalService,
                     View &view);
    void init();
public slots:
    void onDebugRequested();
    void onWindowStateChanged(const WindowDTO &dto);
    void onEditorStateChanged(const EditorVisibleLinesDTO &dto);
    void onEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void onEditorKeyPressed(const EditorKeyPressDTO &dto);

private:
    void sendStateToEditor();
    void sendCursorPosToEditor();
    bool handleTerminalKeyPress(const EditorKeyPressDTO &dto);
    void handleEditorKeyPress(const EditorKeyPressDTO &dto);
    void executeCommand();
    IModelAccessRead &m_modelAccess;
    WindowService &m_windowService;
    EditorService &m_editorService;
    TerminalService &m_terminalService;
    View &m_view;
    bool m_isTerminal = false;
    QProcess m_process;
    void printModel() const;
    mutable int debugPrintCounter = 0;
};
