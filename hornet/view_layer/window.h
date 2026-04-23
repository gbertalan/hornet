#pragma once

#include <QWidget>

struct EditorCursorPosDTO;
struct WindowDTO;
struct NumberDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
class TitleBar;
class ResizeHandle;
class SplitPane;
class OverlayWidget;

class Window : public QWidget {
    Q_OBJECT

public:
    explicit Window(const WindowDTO& initialState, QWidget* parent = nullptr);
    void displayNumber(const NumberDTO& dto);
    void restoreWindowedSize();
    void restoreWindowedLocation();
    void updateEditorLines(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);

signals:
    void buttonClicked();
    void debugRequested();
    void windowStateChanged(const WindowDTO& dto);
    void closeClicked();
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorUserInputOccured(const EditorCursorPosDTO &dto);

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

    int m_windowedWidth;
    int m_windowedHeight;
    int m_windowedX;
    int m_windowedY;

    // Creates the resize handles, small, transparent panels to handle the edge/corner dragging
    void setupResizeHandles();
    // Places the resize handlers to the edges and corners
    void positionResizeHandles();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    // Disable resize when in fullscreen mode
    void changeEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
};
