#pragma once

#include <QWidget>
#include <QPoint>

#include "view_layer/window.h"

class TitlebarButton;

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = nullptr);
    void setFullscreen(bool fullscreen);

signals:
    void closeClicked();
    void minimizeClicked();
    void maximizeClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_dragging;
    bool m_doubleclicked;
    double m_grabRatio;
    QPoint m_dragStartPosition;
    Window* m_window;
    TitlebarButton* m_closeButton;
    TitlebarButton* m_maxMinButton;
    TitlebarButton* m_trayButton;
};
