#ifndef GRID_H
#define GRID_H
#include <QPoint>
#include <QWidget>
#include "shared/dto_model_to_view/boxviewdto.h"
#include <memory>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

struct GridDragDTO;
struct GridViewStateDTO;
struct GridZoomDTO;
struct BoxDragDTO;

class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(const GridViewStateDTO &initialState, QWidget *parent);
    void updateGridViewState(const GridViewStateDTO &dto);

signals:
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);
    void boxDragged(const BoxDragDTO &dto);

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

    // drag interaction state, local to the view only:
    QPoint m_lastMousePos;
    bool m_isDragging = false;
    int m_hoveredBoxId = -1;
    bool m_isDraggingGrid = false;
    bool m_isDraggingBox = false;
    int m_draggedBoxId = -1;
    QPoint m_dragStartMousePos;
    QPoint m_draggedBoxLiveOffset; // how much we are dragging right now

    // text rendering:
    FontAtlas m_fontAtlas;
    std::unique_ptr<FontRenderer> m_fontRenderer;
};
#endif // GRID_H
