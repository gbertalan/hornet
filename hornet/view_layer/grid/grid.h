#ifndef GRID_H
#define GRID_H
#include <QPoint>
#include <QWidget>
#include "shared/dto_model_to_view/boxviewdto.h"
#include "shared/dto_view_to_model/boxresizeedge.h"
#include <memory>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>
struct BoxResizeDTO;
struct GridDragDTO;
struct GridViewStateDTO;
struct GridZoomDTO;
struct BoxDragDTO;
struct BoxSelectedDTO;
class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(const GridViewStateDTO &initialState,
                  FontAtlas &fontAtlas,
                  FontRenderer &fontRenderer,
                  QWidget *parent);
    void updateGridViewState(const GridViewStateDTO &dto);

    // ================================================================
    // SLICE: caret / Ctrl-state sync (direct wiring from Editor, no Control)
    // ================================================================
    void setCursorBlinkVisible(bool visible);
public slots:
    void setCtrlPressed(bool isCtrlPressed);
signals:
    // ================================================================
    // SLICE: grid viewport (zoom, pan)
    // ================================================================
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);

    // ================================================================
    // SLICE: box manipulation (drag, select, resize, unload)
    // ================================================================
    void boxDragged(const BoxDragDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);
    void boxResized(const BoxResizeDTO &dto);
    void boxUnloadRequested(int boxId);

protected:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // cached render state, mirrors GridModel via GridViewStateDTO
    double gridGap = 30.0;
    QPoint offset = {0, 0};
    std::vector<BoxViewDTO> boxes;

    // ================================================================
    // SLICE: drag / hover / resize interaction state (view-local only,
    // not part of GridModel)
    // ================================================================
    QPoint m_lastMousePos;
    bool m_isDragging = false;
    int m_hoveredBoxId = -1;
    bool m_isDraggingGrid = false;
    bool m_isDraggingBox = false;
    int m_draggedBoxId = -1;
    QPoint m_dragStartMousePos;
    QPoint m_draggedBoxLiveOffset; // how much we are dragging right now
    int m_selectedBoxId = -1;
    bool m_cursorBlinkVisible = true;
    static constexpr int m_clickDistanceThreshold
        = 5; // pixels; press+release under this = a click, not a drag
    bool m_isResizingBox = false;
    int m_resizedBoxId = -1;
    BoxResizeEdge m_resizeEdge = BoxResizeEdge::None;
    QPoint m_resizeDragStartMousePos;
    QPoint m_lastAppliedResizeCellDelta;

    // text rendering:
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    Qt::CursorShape cursorForResizeEdge(BoxResizeEdge edge) const;
    bool m_isCtrlPressed = false;
};
#endif // GRID_H
