#pragma once

#include <QObject>

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

signals:
    void buttonClicked();
    void windowStateChanged(const WindowDTO& dto);
    void closeClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);
    void boxDragged(const BoxDragDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);

private:
    Window* m_window;
};
