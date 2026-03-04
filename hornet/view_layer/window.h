#pragma once

#include <QWidget>


struct NumberDTO;
class TitleBar;
class ResizeHandle;
class LowerWidget;
class OverlayWidget;

class Window : public QWidget {
    Q_OBJECT

public:
    explicit Window(QWidget* parent = nullptr);
    void displayNumber(const NumberDTO& dto);
    void restoreWindowedSize();

signals:
    void buttonClicked();
    void windowStateChanged(int x, int y, int width, int height, bool isFullscreen);

private:
    TitleBar* m_titleBar;
    LowerWidget* m_lowerWidget;
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

    // Creates the resize handles, small, transparent panels to handle the edge/corner dragging
    void setupResizeHandles();
    // Places the resize handlers to the edges and corners
    void positionResizeHandles();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    // Disable resize when in fullscreen mode
    void changeEvent(QEvent* event) override;
};
