#include "grid.h"
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>

Grid::Grid(QWidget *parent)
    : QWidget(parent)
{}

void Grid::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const double scaledGap = gridGap * scale;
    const double startX = std::fmod(offset.x(), scaledGap);
    const double startY = std::fmod(offset.y(), scaledGap);

    painter.setPen(QPen(Qt::white, 1));

    const int verticalLines = static_cast<int>(std::ceil((width() - startX) / scaledGap)) + 1;
    for (int i = 0; i < verticalLines; ++i) {
        const double x = startX + i * scaledGap;
        painter.drawLine(QPointF(x, 0), QPointF(x, height()));
    }

    const int horizontalLines = static_cast<int>(std::ceil((height() - startY) / scaledGap)) + 1;
    for (int i = 0; i < horizontalLines; ++i) {
        const double y = startY + i * scaledGap;
        painter.drawLine(QPointF(0, y), QPointF(width(), y));
    }
}

void Grid::wheelEvent(QWheelEvent *event)
{
    const int delta = event->angleDelta().y();
    qDebug() << "wheel delta:" << delta << "position:" << event->position();
    event->accept();
}
