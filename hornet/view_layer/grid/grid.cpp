#include "grid.h"
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include "canvaspainter.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/boxresizeedge.h"
#include "shared/dto_view_to_model/boxselecteddto.h"
#include "shared/dto_view_to_model/gridzoomdto.h"
#include <cmath>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/boxresizedto.h>
#include <shared/dto_view_to_model/griddragdto.h>

Grid::Grid(const GridViewStateDTO &initialState, QWidget *parent)
    : QWidget(parent)
    , gridGap(initialState.gridGap)
    , offset(initialState.offset)
    , boxes(initialState.boxes)
{
    setMouseTracking(true);

    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);
}
void Grid::updateGridViewState(const GridViewStateDTO &dto)
{
    gridGap = dto.gridGap;
    offset = dto.offset;
    boxes = dto.boxes;
    m_selectedBoxId = dto.selectedBoxId;

    update();
}

void Grid::setCursorBlinkVisible(bool visible)
{
    m_cursorBlinkVisible = visible;
    update();
}

void Grid::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    CanvasPainter::drawGrid(painter, gridGap, offset, size());
    CanvasPainter::drawBoxes(painter,
                             gridGap,
                             offset,
                             boxes,
                             m_hoveredBoxId,
                             m_selectedBoxId,
                             m_isDraggingBox ? m_draggedBoxId : -1,
                             m_draggedBoxLiveOffset,
                             *m_fontRenderer,
                             m_fontAtlas,
                             m_cursorBlinkVisible);
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
        int resizeBoxId = -1;
        const BoxResizeEdge resizeEdge = CanvasPainter::findResizeEdgeAtPosition(event->pos(),
                                                                                 gridGap,
                                                                                 offset,
                                                                                 boxes,
                                                                                 resizeBoxId);
        if (resizeEdge != BoxResizeEdge::None) { // resizing a box
            m_isResizingBox = true;
            m_resizedBoxId = resizeBoxId;
            m_resizeEdge = resizeEdge;
            m_resizeDragStartMousePos = event->pos();
            m_lastAppliedResizeCellDelta = QPoint(0, 0);
        } else {
            const int boxIdUnderCursor = CanvasPainter::findBoxAtPosition(event->pos(),
                                                                          gridGap,
                                                                          offset,
                                                                          boxes);
            if (boxIdUnderCursor != -1) { // dragging a box
                m_isDraggingBox = true;
                m_draggedBoxId = boxIdUnderCursor;
                m_draggedBoxLiveOffset = QPoint(0, 0); // resetting this when box drag starts.
            } else {                                   // dragging the whole grid
                m_isDraggingGrid = true;
            }
        }
        m_dragStartMousePos = event->pos();
        m_lastMousePos = event->pos();
    }
    event->accept();
}

void Grid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isResizingBox) {
        const QPoint totalPixelDelta = event->pos() - m_resizeDragStartMousePos;
        const QPoint totalCellDelta(static_cast<int>(std::round(totalPixelDelta.x() / gridGap)),
                                    static_cast<int>(std::round(totalPixelDelta.y() / gridGap)));
        if (totalCellDelta != m_lastAppliedResizeCellDelta) {
            const QPoint incrementalCellDelta = totalCellDelta - m_lastAppliedResizeCellDelta;
            m_lastAppliedResizeCellDelta = totalCellDelta;
            emit boxResized(BoxResizeDTO(m_resizedBoxId, m_resizeEdge, incrementalCellDelta));
        }
    } else if (m_isDraggingGrid) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        emit gridDragged(GridDragDTO(delta, event->pos()));
    } else if (m_isDraggingBox) {
        const BoxViewDTO *draggedBox = nullptr;
        for (const BoxViewDTO &box : boxes)
            if (box.id == m_draggedBoxId)
                draggedBox = &box;

        if (draggedBox) {
            const QRectF rectBeforeMove = CanvasPainter::getBoxScreenRect(*draggedBox,
                                                                          gridGap,
                                                                          offset,
                                                                          m_draggedBoxLiveOffset);
            m_draggedBoxLiveOffset = event->pos() - m_dragStartMousePos;
            const QRectF rectAfterMove = CanvasPainter::getBoxScreenRect(*draggedBox,
                                                                         gridGap,
                                                                         offset,
                                                                         m_draggedBoxLiveOffset);

            constexpr int repaintMarginPixels = 1;
            const QRect dirtyRect = (rectBeforeMove | rectAfterMove)
                                        .toAlignedRect()
                                        .adjusted(-repaintMarginPixels,
                                                  -repaintMarginPixels,
                                                  repaintMarginPixels,
                                                  repaintMarginPixels);
            update(dirtyRect);
        }
    } else { // hover detection:
        int resizeBoxId = -1;
        const BoxResizeEdge resizeEdge = CanvasPainter::findResizeEdgeAtPosition(event->pos(),
                                                                                 gridGap,
                                                                                 offset,
                                                                                 boxes,
                                                                                 resizeBoxId);
        setCursor(cursorForResizeEdge(resizeEdge));

        const int boxIdUnderCursor = CanvasPainter::findBoxAtPosition(event->pos(),
                                                                      gridGap,
                                                                      offset,
                                                                      boxes);
        if (boxIdUnderCursor != m_hoveredBoxId) {
            m_hoveredBoxId = boxIdUnderCursor;
            update();
        }
    }
    event->accept();
}

void Grid::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isDraggingBox) {
            const QPoint totalDisplacement = event->pos() - m_dragStartMousePos;
            const bool wasClick = totalDisplacement.manhattanLength() < m_clickDistanceThreshold;
            if (wasClick) {
                emit boxSelected(BoxSelectedDTO(m_draggedBoxId));
            } else
                emit boxDragged(BoxDragDTO({m_draggedBoxId}, totalDisplacement));
        }
        m_isDraggingGrid = false;
        m_isDraggingBox = false;
        m_isResizingBox = false;
        m_resizedBoxId = -1;
        m_resizeEdge = BoxResizeEdge::None;
        m_draggedBoxId = -1;
        m_draggedBoxLiveOffset = QPoint(0, 0);
        update();
    }
    event->accept();
}

Qt::CursorShape Grid::cursorForResizeEdge(BoxResizeEdge edge) const
{
    switch (edge) {
    case BoxResizeEdge::Left:
    case BoxResizeEdge::Right:
        return Qt::SizeHorCursor;
    case BoxResizeEdge::Top:
    case BoxResizeEdge::Bottom:
        return Qt::SizeVerCursor;
    case BoxResizeEdge::TopLeft:
    case BoxResizeEdge::BottomRight:
        return Qt::SizeFDiagCursor;
    case BoxResizeEdge::TopRight:
    case BoxResizeEdge::BottomLeft:
        return Qt::SizeBDiagCursor;
    case BoxResizeEdge::None:
    default:
        return Qt::ArrowCursor;
    }
}
