#pragma once

#include <QWidget>

class OverlayWidget : public QWidget {
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget* parent = nullptr);
    void setFullscreen(bool fullscreen);
    void setFocused(bool focused);
    void setDimmed(bool dimmed);

signals:
    void clickedWhileDimmed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_fullscreen;
    bool m_focused;
    bool m_dimmed = false;
};
