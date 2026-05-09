#include "grid.h"
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include "canvaspainter.h"
#include "shared/dto_view_to_model/gridzoomdto.h"
#include <cmath>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/griddragdto.h>

Grid::Grid(const GridViewStateDTO &initialState, QWidget *parent)
    : QWidget(parent)
    , gridGap(initialState.gridGap)
    , offset(initialState.offset)
{}

void Grid::updateGridViewState(const GridViewStateDTO &dto)
{
    gridGap = dto.gridGap;
    offset = dto.offset;
    boxes = dto.boxes;
    update();
}

void Grid::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    CanvasPainter::drawGrid(painter, gridGap, offset, size());
    CanvasPainter::drawBoxes(painter, gridGap, offset, boxes);
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
    }
    event->accept();
}

void Grid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isDragging) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        emit gridDragged(GridDragDTO(delta, event->pos()));
    }
    event->accept();
}

void Grid::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_isDragging = false;
    event->accept();
}
