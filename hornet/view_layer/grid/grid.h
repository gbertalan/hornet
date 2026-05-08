#ifndef GRID_H
#define GRID_H
#include <QPoint>
#include <QWidget>
#include "shared/dto_bidirectional/gridzoomdto.h"

class Grid : public QWidget
{
    Q_OBJECT

public:
    explicit Grid(QWidget *parent);

signals:
    void gridZoomChanged(const GridZoomDTO &dto);

protected:
    void paintEvent(QPaintEvent *) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    float gridGap = 30.0f;
    double scale = 1.0;
    QPoint offset = {0, 0};
};
#endif // GRID_H
