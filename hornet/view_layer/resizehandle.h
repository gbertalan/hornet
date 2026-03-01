#pragma once

#include <QWidget>

enum class ResizeEdge {
    Left, Right, Top, Bottom,
    TopLeft, TopRight, BottomLeft, BottomRight
};

    // Small, transparent panels to handle the edge/corner dragging
class ResizeHandle : public QWidget {
    Q_OBJECT

public:
    explicit ResizeHandle(ResizeEdge edge, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    ResizeEdge m_edge;
    bool m_resizing;
    QPoint m_startPos;
    QRect m_startGeometry;
};
