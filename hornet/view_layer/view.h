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
struct BoxListPageDTO;
struct BoxListPageRequestDTO;

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
    void updateFileLoaderBoxListPage(const BoxListPageDTO &dto);
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
    void boxUnloadRequested(int boxId);
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void fileLoaderBoxListPageRequested(const BoxListPageRequestDTO &dto);

private:
    Window* m_window;
};
