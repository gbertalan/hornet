#pragma once
#include <QDebug>
#include <QObject>
#include "control_layer/editorcontrol.h"
#include "control_layer/gridcontrol.h"
#include "control_layer/terminalcontrol.h"
#include "control_layer/windowcontrol.h"
#include <filesystem>

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
struct BoxSelectedDTO;
struct BoxResizeDTO;

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
    void onBoxSelected(const BoxSelectedDTO &dto);
    void onBoxResized(const BoxResizeDTO &dto);
    // debug:
    void onDebugRequested();
    QString saveProjectToFile(const std::filesystem::path &filePath);
    void onBoxUnloadRequested(int boxId);

private:
    /**
     * @brief buildTerminalPrompts Reads ALL terminal prompt-lines from Model and converts them to QString,
     * in the same order as the terminal lines in the editor buffer.
     */
    QVector<QString> buildTerminalPrompts() const;
    int m_currentlySelectedBoxId = -1;
    int m_lastCreatedBoxId = -1;
    void flushEditorContentToBox(int boxId);
    QString convertU32StringToQString(const std::u32string &text) const;
    std::u32string convertQStringToU32String(const QString &text) const;
    std::vector<std::u32string> convertBodyLinesToU32(const QVector<QString> &bodyLines) const;
    void printModel() const;

    IModelAccessRead &m_modelAccess;

    EditorService &m_editorService;
    TerminalService &m_terminalService;
    GridService &m_gridService;

    WindowControl m_windowControl;
    EditorControl m_editorControl;
    TerminalControl m_terminalControl;
    GridControl m_gridControl;

    // Avoids redundant grid refreshes when the Editor reports the same scroll
    // position it already reported last time. Reset to -1 on box switch, since
    // a cached value from a different box must not suppress the first sync for
    // the newly selected one.
    int m_lastSyncedBoxScrollOffset = -1;

    bool loadFileIntoNewBox(const std::filesystem::path &filePath);
    QString dispatchHornetCommand(const HornetCommandDTO &command);
    QString executeScriptFile(const std::filesystem::path &filePath,
                              const std::filesystem::path &workingDir,
                              int depth);
    void createCommandOutputBox(const QString &commandText, const QString &outputText);

    bool m_isRestoringBoxState = false;

    bool resolveBoxIdToken(const QString &token, int &outBoxId) const;

    mutable int debugPrintCounter = 0;
};
