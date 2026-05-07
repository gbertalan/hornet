#ifndef GRID_H
#define GRID_H
#include <QPointF>
#include <QWidget>

class Grid : public QWidget
{
public:
    explicit Grid(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float gridGap = 30.0f;
    float scale = 1.0f;
    QPointF offset = {0, 0};
};
#endif // GRID_H
