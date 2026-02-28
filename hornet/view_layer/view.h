#pragma once

#include <QWidget>


struct NumberDTO;
class TitleBar;
class ResizeHandle;
class LowerWidget;
class OverlayWidget;

class View : public QWidget {
    Q_OBJECT

public:
    explicit View(QWidget* parent = nullptr);
    void displayNumber(const NumberDTO& dto);
    void showError(const QString& message);

signals:
    void buttonClicked();

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

    void setupResizeHandles();
    void positionResizeHandles();

protected:
    void resizeEvent(QResizeEvent* event) override;
};
