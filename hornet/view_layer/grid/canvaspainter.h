#ifndef CANVASPAINTER_H
#define CANVASPAINTER_H
#include <QPainter>
#include <QPoint>
#include "shared/dto_model_to_view/boxviewdto.h"
#include <vector>
#include <view_layer/font_renderer/FontRenderer.h>
class CanvasPainter
{
public:
    static void drawGrid(QPainter &painter, double gridGap, QPoint offset, QSize size);
    static void drawBoxes(QPainter &painter,
                          double gridGap,
                          QPoint offset,
                          const std::vector<BoxViewDTO> &boxes,
                          int hoveredBoxId,
                          int draggedBoxId,
                          QPoint draggedBoxLiveOffset,
                          FontRenderer &fontRenderer,
                          FontAtlas &fontAtlas);
    static int findBoxAtPosition(QPoint mousePosition,
                                 double gridGap,
                                 QPoint offset,
                                 const std::vector<BoxViewDTO> &boxes);
};
#endif // CANVASPAINTER_H
