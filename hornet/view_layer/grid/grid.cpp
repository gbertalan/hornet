#include "grid.h"
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>

Grid::Grid(QWidget *parent)
    : QWidget(parent)
{}

void Grid::updateGridViewState(const GridViewStateDTO &dto)
{
    gridGap = dto.gridGap;
    offset = dto.offset;
    update();
}

void Grid::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const double startX = std::fmod(offset.x(), gridGap);
    const double startY = std::fmod(offset.y(), gridGap);

    painter.setPen(QPen(Qt::white, 1));

    const int verticalLines = static_cast<int>(std::ceil((width() - startX) / gridGap)) + 1;
    for (int i = 0; i < verticalLines; ++i) {
        const double x = startX + i * gridGap;
        painter.drawLine(QPointF(x, 0), QPointF(x, height()));
    }

    const int horizontalLines = static_cast<int>(std::ceil((height() - startY) / gridGap)) + 1;
    for (int i = 0; i < horizontalLines; ++i) {
        const double y = startY + i * gridGap;
        painter.drawLine(QPointF(0, y), QPointF(width(), y));
    }
}

void Grid::wheelEvent(QWheelEvent *event)
{
    const ScrollDirection direction = event->angleDelta().y() > 0 ? ScrollDirection::Up
                                                                  : ScrollDirection::Down;

    emit gridZoomChanged(GridZoomDTO(direction, event->position().toPoint()));
    event->accept();
}

void Grid::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        qDebug() << "drag started at:" << event->pos();
    }
    event->accept();
}

void Grid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        qDebug() << "drag delta:" << delta;
    }
    event->accept();
}

void Grid::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        qDebug() << "drag ended";
    }
    event->accept();
}
