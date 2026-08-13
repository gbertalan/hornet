#pragma once

#include <QWidget>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

struct BoxResizeDTO;
struct BoxSelectedDTO;
struct BoxDragDTO;
struct GridDragDTO;
struct GridViewStateDTO;
struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
struct WindowDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
struct EditorSettingsDTO;
struct GridZoomDTO;
class TitleBar;
class ResizeHandle;
class SplitPane;
class OverlayWidget;
class TitlebarFileDropdown;
class MainPopup;
class FileLoaderPanel;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
struct FileLoadRequestDTO;

class Window : public QWidget {
    Q_OBJECT

public:
    explicit Window(const WindowDTO& initialState, QWidget* parent = nullptr);
    void restoreWindowedSize();
    void restoreWindowedLocation();
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
    void fileLoaderLoadRequested(const FileLoadRequestDTO &dto);

private:
    TitleBar* m_titleBar;
    SplitPane* m_splitPane;
    OverlayWidget* m_overlayWidget;
    ResizeHandle* m_handleLeft;
    ResizeHandle* m_handleRight;
    ResizeHandle* m_handleTop;
    ResizeHandle* m_handleBottom;
    ResizeHandle* m_handleTopLeft;
    ResizeHandle* m_handleTopRight;
    ResizeHandle* m_handleBottomLeft;
    ResizeHandle* m_handleBottomRight;

    FontAtlas m_fontAtlas;
    std::unique_ptr<FontRenderer> m_fontRenderer;

    int m_windowedWidth;
    int m_windowedHeight;
    int m_windowedX;
    int m_windowedY;

    // Creates the resize handles, small, transparent panels to handle the edge/corner dragging
    void setupResizeHandles();
    // Places the resize handlers to the edges and corners
    void positionResizeHandles();

    TitlebarFileDropdown *m_fileDropdown;

    MainPopup *m_mainPopup;
    FileLoaderPanel *m_fileLoaderPanel;
    void positionMainPopup();
    void openFileLoadPopup();
    void openProjectSavePopup();
    void openScriptRunPopup();
    void openMainPopupShared();
    void closeMainPopup();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    // Disable resize when in fullscreen mode
    void changeEvent(QEvent* event) override;
};
