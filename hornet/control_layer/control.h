#pragma once
#include <QDebug>
#include <QObject>
#include "control_layer/editorcontrol.h"
#include "control_layer/gridcontrol.h"
#include "control_layer/terminalcontrol.h"
#include "control_layer/windowcontrol.h"

class IModelAccessRead;
class WindowService;
class EditorService;
class TerminalService;
class GridService;
class View;

struct EditorKeyPressDTO;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
struct GridZoomDTO;
struct GridDragDTO;
struct BoxDragDTO;

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(IModelAccessRead &modelAccess,
                     WindowService &windowService,
                     EditorService &editorService,
                     TerminalService &terminalService,
                     GridService &gridService,
                     View &view);
    void init();
public slots:
    // window:
    void onWindowStateChanged(const WindowDTO &dto);
    // editor:
    void onEditorStateChanged(const EditorVisibleLinesDTO &dto);
    void onEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void onEditorKeyPressed(const EditorKeyPressDTO &dto);
    // grid:
    void onGridZoomChanged(const GridZoomDTO &dto);
    void onGridDrag(const GridDragDTO &dto);
    void onBoxDragged(const BoxDragDTO &dto);
    // debug:
    void onDebugRequested();

private:
    /**
     * @brief buildTerminalPrompts Reads ALL terminal prompt-lines from Model and converts them to QString,
     * in the same order as the terminal lines in the editor buffer.
     */
    QVector<QString> buildTerminalPrompts() const;
    void printModel() const;

    IModelAccessRead &m_modelAccess;

    EditorService &m_editorService;
    TerminalService &m_terminalService;
    GridService &m_gridService;

    WindowControl m_windowControl;
    EditorControl m_editorControl;
    TerminalControl m_terminalControl;
    GridControl m_gridControl;

    mutable int debugPrintCounter = 0;
};
