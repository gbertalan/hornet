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
    /**
     * @brief getBoxScreenRect Gives us the rect where the box is, for efficient repaint
     * @param box
     * @param gridGap
     * @param offset
     * @param liveOffset
     * @return 
     */
    static QRectF getBoxScreenRect(const BoxViewDTO &box,
                                   double gridGap,
                                   QPoint offset,
                                   QPoint liveOffset = QPoint(0, 0));
};
#endif // CANVASPAINTER_H
