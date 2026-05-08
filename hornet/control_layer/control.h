#pragma once
#include <QDebug>
#include <QObject>
#include "control_layer/editorcontrol.h"
#include "control_layer/gridcontrol.h"
#include "control_layer/terminalcontrol.h"
#include "control_layer/windowcontrol.h"
struct EditorKeyPressDTO;
class IModelAccessRead;
class WindowService;
class EditorService;
class TerminalService;
class GridService;
class View;
struct EditorVisibleLinesDTO;
struct EditorCursorPosDTO;
struct GridZoomDTO;

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
    void onDebugRequested();
    void onWindowStateChanged(const WindowDTO &dto);
    void onEditorStateChanged(const EditorVisibleLinesDTO &dto);
    void onEditorCursorPosChanged(const EditorCursorPosDTO &dto);
    void onEditorKeyPressed(const EditorKeyPressDTO &dto);
    void onGridZoomChanged(const GridZoomDTO &dto);

private:
    QVector<QString> buildTerminalPrompts() const;
    IModelAccessRead &m_modelAccess;
    EditorService &m_editorService;
    TerminalService &m_terminalService;
    GridService &m_gridService;
    WindowControl m_windowControl;
    EditorControl m_editorControl;
    TerminalControl m_terminalControl;
    GridControl m_gridControl;
    void printModel() const;
    mutable int debugPrintCounter = 0;
};
