#pragma once

#include <QWidget>
#include <QPoint>

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = nullptr);

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
};
