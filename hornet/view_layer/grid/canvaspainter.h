#ifndef CANVASPAINTER_H
#define CANVASPAINTER_H

#include <QPainter>
#include <QPoint>
#include "model_layer/boxmodel.h"
#include <vector>
#include <view_layer/font_renderer/FontRenderer.h>

class CanvasPainter
{
public:
    static void drawGrid(QPainter &painter, double gridGap, QPoint offset, QSize size);
    static void drawBoxes(QPainter &painter,
                          double gridGap,
                          QPoint offset,
                          const std::vector<BoxModel> &boxes,
                          FontRenderer &fontRenderer);
};
#endif // CANVASPAINTER_H
