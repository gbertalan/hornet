#ifndef GRID_H
#define GRID_H
#include <QPoint>
#include <QWidget>

#include <shared/dto_model_to_view/gridviewstatedto.h>

#include <shared/dto_view_to_model/gridzoomdto.h>

class Grid : public QWidget
{
    Q_OBJECT

public:
    explicit Grid(QWidget *parent);
    void updateGridViewState(const GridViewStateDTO &dto);

signals:
    void gridZoomChanged(const GridZoomDTO &dto);

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
};
#endif // GRID_H
