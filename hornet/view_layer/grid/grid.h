#ifndef GRID_H
#define GRID_H
#include <QPoint>
#include <QWidget>
#include "model_layer/boxmodel.h"

struct GridDragDTO;
struct GridViewStateDTO;
struct GridZoomDTO;
class Grid : public QWidget
{
    Q_OBJECT

public:
    explicit Grid(const GridViewStateDTO &initialState, QWidget *parent);
    void updateGridViewState(const GridViewStateDTO &dto);

signals:
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);

protected:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    double gridGap = 30.0;
    QPoint offset = {0, 0};
    QPoint m_lastMousePos;
    bool m_isDragging = false;
    std::vector<BoxModel> boxes;
};
#endif // GRID_H
