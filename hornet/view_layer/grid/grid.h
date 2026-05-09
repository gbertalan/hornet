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

private:
    double gridGap = 30.0;
    double scale = 1.0;
    QPoint offset = {0, 0};
};
#endif // GRID_H
