#pragma once
#include <QDebug>
#include <QObject>
#include "control_layer/editorcontrol.h"
#include "control_layer/gridcontrol.h"
#include "control_layer/terminalcontrol.h"
#include "control_layer/toolcontrol.h"
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
struct BoxListPageRequestDTO;
struct FilePathListDTO;

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
    // ================================================================
    // SLICE: window
    // ================================================================
    void onWindowStateChanged(const WindowDTO &dto);

    // ================================================================
    // SLICE: editor <-> model sync (state/cursor)
    // ================================================================
    void onEditorStateChanged(const EditorVisibleLinesDTO &dto);
    void onEditorCursorPosChanged(const EditorCursorPosDTO &dto);

    // ================================================================
    // SLICE: editor key dispatch (typing, plus terminal/hornet routing)
    // ================================================================
    void onEditorKeyPressed(const EditorKeyPressDTO &dto);

    // ================================================================
    // SLICE: grid viewport (zoom, pan)
    // ================================================================
    void onGridZoomChanged(const GridZoomDTO &dto);
    void onGridDrag(const GridDragDTO &dto);

    // ================================================================
    // SLICE: box manipulation (drag, select, resize)
    // ================================================================
    void onBoxDragged(const BoxDragDTO &dto);
    void onBoxSelected(const BoxSelectedDTO &dto);
    void onBoxResized(const BoxResizeDTO &dto);

    // Note: the two below are not actually connected as Qt slots today -
    // saveProjectToFile is called directly from dispatchHornetCommand, and
    // onBoxUnloadRequested IS a real slot (connected in main.cpp). Left in
    // this section unchanged for now - worth revisiting whether
    // saveProjectToFile belongs in "public slots" at all.
    QString saveProjectToFile(const std::filesystem::path &filePath);

    // ================================================================
    // SLICE: box unload (close button - separate from "hornet unload")
    // ================================================================
    void onBoxUnloadRequested(int boxId);

    // ================================================================
    // SLICE: box list paging (titlebar dropdown, file loader popup)
    // ================================================================
    void onBoxListPageRequested(const BoxListPageRequestDTO &dto);
    void onPopupBoxListPageRequested(const BoxListPageRequestDTO &dto);
    void onFileLoaderLoadRequested(const FilePathListDTO &dto);
    void onProjectSaverSaveRequested(const QString &baseName);
    void onScriptRunnerBoxRunRequested(int boxId);
    void onScriptRunnerRunRequested(const FilePathListDTO &dto);

    // ================================================================
    // SLICE: debug
    // ================================================================
    void onDebugRequested();

private:
    /**
     * @brief buildTerminalPrompts Reads ALL terminal prompt-lines from Model and converts them to QString,
     * in the same order as the terminal lines in the editor buffer.
     */
    QVector<QString> buildTerminalPrompts() const;
    int m_currentlySelectedBoxId = -1;
    std::vector<int> m_recentlyCreatedBoxIds;

    // ================================================================
    // SLICE: editor <-> box sync helpers
    // ================================================================
    void flushEditorContentToBox(int boxId);

    // ================================================================
    // SLICE: type conversion helpers (u32string <-> QString)
    // ================================================================
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
    ToolControl m_toolControl;

    // Avoids redundant grid refreshes when the Editor reports the same scroll
    // position it already reported last time. Reset to -1 on box switch, since
    // a cached value from a different box must not suppress the first sync for
    // the newly selected one.
    int m_lastSyncedBoxScrollOffset = -1;

    // ================================================================
    // SLICE: hornet command system (load, save, scripting, metadata commands)
    // ================================================================
    bool loadFileIntoNewBox(const std::filesystem::path &filePath);
    QString dispatchHornetCommand(const HornetCommandDTO &command);
    QString executeScriptFile(const std::filesystem::path &filePath,
                              const std::filesystem::path &workingDir,
                              int depth);
    void createCommandOutputBox(const QString &commandText, const QString &outputText);
    bool resolveBoxIdToken(const QString &token, int &outBoxId) const;
    void selectBox(int boxId);

    bool m_isRestoringBoxState = false;
    mutable int debugPrintCounter = 0;
};
