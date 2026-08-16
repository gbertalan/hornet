#pragma once

#include <QObject>

struct BoxResizeDTO;
struct BoxSelectedDTO;
struct BoxDragDTO;
struct GridDragDTO;
struct GridViewStateDTO;
struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
struct EditorSettingsDTO;
struct WindowDTO;
struct GridZoomDTO;
class MainPopup;
class FileLoaderPanel;
class ScriptRunnerPanel;
class ProjectSaverPanel;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
struct FilePathListDTO;
struct BoxUnloadRequestedDTO;
struct ToolButtonActivatedDTO;

class Window;

class View : public QObject {
    Q_OBJECT

public:
    explicit View(const WindowDTO& initialState, QObject* parent = nullptr);
    void show();

    void updateEditorState(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);
    void updateEditorSettings(const EditorSettingsDTO &dto);
    void updateGridViewState(const GridViewStateDTO &dto);
    void updateFileName(const QString &fileName);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void updatePopupBoxListPage(const BoxListPageDTO &dto);
    void updateProjectSaverSaveResult(const QString &message);
    void updateCurrentBoxId(int boxId);

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void windowCloseClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);
    void boxDragged(const BoxDragDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);
    void boxResized(const BoxResizeDTO &dto);
    void boxUnloadRequested(const BoxUnloadRequestedDTO &dto);
    void toolButtonActivated(const ToolButtonActivatedDTO &dto);
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void popupBoxListPageRequested(const BoxListPageRequestDTO &dto);
    void fileLoaderLoadRequested(const FilePathListDTO &dto);
    void scriptRunnerBoxRunRequested(int boxId);
    void scriptRunnerRunRequested(const FilePathListDTO &dto);
    void projectSaverSaveRequested(const QString &baseName);

private:
    Window* m_window;
};
