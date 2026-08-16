#include "grid.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QWheelEvent>
#include "canvaspainter.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/boxresizeedge.h"
#include "shared/dto_view_to_model/boxselecteddto.h"
#include "shared/dto_view_to_model/boxunloadrequesteddto.h"
#include "shared/dto_view_to_model/gridzoomdto.h"
#include "shared/dto_view_to_model/toolbuttonactivateddto.h"
#include <cmath>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/boxresizedto.h>
#include <shared/dto_view_to_model/griddragdto.h>

// ================================================================
// SLICE: construction & initialization
// ================================================================

Grid::Grid(const GridViewStateDTO &initialState,
           FontAtlas &fontAtlas,
           FontRenderer &fontRenderer,
           QWidget *parent)
    : QWidget(parent)
    , gridGap(initialState.gridGap)
    , offset(initialState.offset)
    , boxes(initialState.boxes)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    setMouseTracking(true);
}

// ================================================================
// SLICE: Model -> View state push (called by Control via GridControl)
// ================================================================

void Grid::updateGridViewState(const GridViewStateDTO &dto)
{
    gridGap = dto.gridGap;
    if (!m_isDraggingGrid)
        offset = dto.offset;
    boxes = dto.boxes;
    m_selectedBoxId = dto.selectedBoxId;

    update();
}

// ================================================================
// SLICE: caret / Ctrl-state sync (direct wiring from Editor, no Control)
// ================================================================

void Grid::setCursorBlinkVisible(bool visible)
{
    m_cursorBlinkVisible = visible;
    for (const BoxViewDTO &box : boxes) {
        if (box.id == m_selectedBoxId) {
            const QRectF boxRect = CanvasPainter::getBoxScreenRect(box, gridGap, offset);
            update(boxRect.toAlignedRect());
            return;
        }
    }
    update();
}

void Grid::setCtrlPressed(bool isCtrlPressed)
{
    m_isCtrlPressed = isCtrlPressed;
    if (!isCtrlPressed && m_hoveredButtonBoxId != -1) {
        m_hoveredButtonBoxId = -1;
        m_hoveredButtonIndex = -1;
    }
    for (const BoxViewDTO &box : boxes) {
        if (box.id == m_hoveredBoxId) {
            const QRectF boxRect = CanvasPainter::getBoxScreenRect(box, gridGap, offset);
            update(boxRect.toAlignedRect());
            return;
        }
    }
    update();
}

// ================================================================
// SLICE: rendering
// ================================================================

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
                             m_fontRenderer,
                             m_fontAtlas,
                             m_cursorBlinkVisible,
                             m_isCtrlPressed,
                             m_hoveredButtonBoxId,
                             m_hoveredButtonIndex,
                             size());
}

// ================================================================
// SLICE: grid viewport (zoom via mouse wheel)
// ================================================================

void Grid::wheelEvent(QWheelEvent *event)
{
    const ScrollDirection direction = event->angleDelta().y() > 0 ? ScrollDirection::Up
                                                                  : ScrollDirection::Down;
    emit gridZoomChanged(GridZoomDTO(direction, event->position().toPoint()));
    event->accept();
}

// ================================================================
// SLICE: mouse interaction - press (decides which drag mode starts:
// close-button click, resize, box-drag, or grid-pan)
// ================================================================

void Grid::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // --- Ctrl held: only chrome (close-X, tool buttons) is interactive.
        // No selection, drag, or resize starts while Ctrl is held. ---
        if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
            const int closeBoxId = CanvasPainter::findBoxCloseButtonAtPosition(event->pos(),
                                                                               gridGap,
                                                                               offset,
                                                                               m_hoveredBoxId,
                                                                               boxes);
            if (closeBoxId != -1) {
                emit boxUnloadRequested(BoxUnloadRequestedDTO(closeBoxId));
                event->accept();
                return;
            }

            int buttonIndex = -1;
            const int buttonBoxId = CanvasPainter::findToolButtonAtPosition(event->pos(),
                                                                            gridGap,
                                                                            offset,
                                                                            m_hoveredBoxId,
                                                                            boxes,
                                                                            buttonIndex);
            if (buttonBoxId != -1) {
                for (const BoxViewDTO &box : boxes) {
                    if (box.id != buttonBoxId)
                        continue;
                    const ToolButtonDTO &toolButton = box.toolScript.buttons.at(buttonIndex);
                    emit toolButtonActivated(
                        ToolButtonActivatedDTO(buttonBoxId, toolButton.hornetCommand));
                    break;
                }
            }
            event->accept();
            return;
        }

        // --- resize edge/corner check ---
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
            // --- box-drag vs. grid-pan ---
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

// ================================================================
// SLICE: mouse interaction - move (continues whichever drag mode
// was started in mousePressEvent, or does hover detection if none)
// ================================================================

void Grid::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isResizingBox) {
        // --- resize: discrete/snap-per-move, incremental cell delta ---
        const QPoint totalPixelDelta = event->pos() - m_resizeDragStartMousePos;
        const QPoint totalCellDelta(static_cast<int>(std::round(totalPixelDelta.x() / gridGap)),
                                    static_cast<int>(std::round(totalPixelDelta.y() / gridGap)));
        if (totalCellDelta != m_lastAppliedResizeCellDelta) {
            const QPoint incrementalCellDelta = totalCellDelta - m_lastAppliedResizeCellDelta;
            m_lastAppliedResizeCellDelta = totalCellDelta;
            emit boxResized(BoxResizeDTO(m_resizedBoxId, m_resizeEdge, incrementalCellDelta));
        }
    } else if (m_isDraggingGrid) {
        // --- grid pan: update local offset immediately, commit to Model only on release ---
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();
        offset += delta;
        update();
    } else if (m_isDraggingBox) {
        // --- box drag: continuous, live pixel offset, snapped to cells only on release ---
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
        // --- resize-cursor hover feedback ---
        int resizeBoxId = -1;
        const BoxResizeEdge resizeEdge = CanvasPainter::findResizeEdgeAtPosition(event->pos(),
                                                                                 gridGap,
                                                                                 offset,
                                                                                 boxes,
                                                                                 resizeBoxId);
        setCursor(cursorForResizeEdge(resizeEdge));

        // --- box hover-highlight tracking ---
        const int boxIdUnderCursor = CanvasPainter::findBoxAtPosition(event->pos(),
                                                                      gridGap,
                                                                      offset,
                                                                      boxes);
        if (boxIdUnderCursor != m_hoveredBoxId) {
            QRectF dirtyRect;
            for (const BoxViewDTO &box : boxes) {
                if (box.id == m_hoveredBoxId || box.id == boxIdUnderCursor) {
                    const QRectF boxRect = CanvasPainter::getBoxScreenRect(box, gridGap, offset);
                    dirtyRect = dirtyRect.isNull() ? boxRect : dirtyRect.united(boxRect);
                }
            }
            m_hoveredBoxId = boxIdUnderCursor;
            if (!dirtyRect.isNull())
                update(dirtyRect.toAlignedRect());
            else
                update();
        }

        // --- tool button hover tracking (only meaningful while Ctrl is held) ---
        if (m_isCtrlPressed) {
            int buttonIndex = -1;
            const int buttonBoxId = CanvasPainter::findToolButtonAtPosition(event->pos(),
                                                                            gridGap,
                                                                            offset,
                                                                            m_hoveredBoxId,
                                                                            boxes,
                                                                            buttonIndex);
            if (buttonBoxId != m_hoveredButtonBoxId || buttonIndex != m_hoveredButtonIndex) {
                m_hoveredButtonBoxId = buttonBoxId;
                m_hoveredButtonIndex = buttonIndex;
                update();
            }
        } else if (m_hoveredButtonBoxId != -1) {
            m_hoveredButtonBoxId = -1;
            m_hoveredButtonIndex = -1;
            update();
        }
    }
    event->accept();
}

// ================================================================
// SLICE: mouse interaction - release (commits whichever drag mode
// was active: click-vs-drag decision, then resets all drag state)
// ================================================================

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
        } else if (m_isDraggingGrid) {
            emit gridDragged(GridDragDTO(offset));
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

// ================================================================
// SLICE: resize cursor-shape helper
// ================================================================

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
