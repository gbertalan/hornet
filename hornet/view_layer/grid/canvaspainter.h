#ifndef CANVASPAINTER_H
#define CANVASPAINTER_H
#include <QPainter>
#include <QPoint>
#include "shared/dto_model_to_view/boxviewdto.h"
#include "shared/dto_view_to_model/boxresizeedge.h"
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
                          int selectedBoxId,
                          int draggedBoxId,
                          QPoint draggedBoxLiveOffset,
                          FontRenderer &fontRenderer,
                          FontAtlas &fontAtlas,
                          bool selectedBoxCursorVisible,
                          bool isCtrlPressed,
                          QSize viewportSize);
    static int findBoxAtPosition(QPoint mousePosition,
                                 double gridGap,
                                 QPoint offset,
                                 const std::vector<BoxViewDTO> &boxes);
    static QRectF getBoxScreenRect(const BoxViewDTO &box,
                                   double gridGap,
                                   QPoint offset,
                                   QPoint liveOffset = QPoint(0, 0));
    static BoxResizeEdge findResizeEdgeAtPosition(QPoint mousePosition,
                                                  double gridGap,
                                                  QPoint offset,
                                                  const std::vector<BoxViewDTO> &boxes,
                                                  int &outBoxId);
    static int findBoxCloseButtonAtPosition(QPoint mousePosition,
                                            double gridGap,
                                            QPoint offset,
                                            int hoveredBoxId,
                                            const std::vector<BoxViewDTO> &boxes);
    static QRectF getBoxCloseButtonRect(const BoxViewDTO &box, double gridGap, QPoint offset);

private:
    struct BoxScreenGeometry
    {
        double screenX;
        double screenY;
        double screenW;
        double screenH;
        double headerH;
    };
    static void drawBoxBackgroundAndBorder(QPainter &painter,
                                           const BoxScreenGeometry &geom,
                                           double edgeThickness,
                                           bool isSelected,
                                           bool isHovered);
    static void drawBoxHeaderText(QPainter &painter,
                                  const BoxViewDTO &box,
                                  const BoxScreenGeometry &geom,
                                  FontRenderer &fontRenderer,
                                  FontAtlas &fontAtlas,
                                  float textScale,
                                  float lineOffset,
                                  float textPadding,
                                  double buttonSize,
                                  double buttonMargin);
    static void drawToolScriptPrimitives(QPainter &painter,
                                         const BoxViewDTO &box,
                                         const BoxScreenGeometry &geom,
                                         double gridGap,
                                         FontRenderer &fontRenderer,
                                         FontAtlas &fontAtlas,
                                         float textScale,
                                         bool isCtrlPressed);
    static void drawToolButtons(QPainter &painter,
                                const BoxViewDTO &box,
                                const BoxScreenGeometry &geom,
                                double gridGap,
                                FontRenderer &fontRenderer,
                                FontAtlas &fontAtlas,
                                float textScale,
                                bool isCtrlPressed);
    static void drawBoxTextContent(QPainter &painter,
                                   const BoxViewDTO &box,
                                   const BoxScreenGeometry &geom,
                                   FontRenderer &fontRenderer,
                                   FontAtlas &fontAtlas,
                                   double gridGap,
                                   float textScale,
                                   float lineOffset,
                                   float textPadding,
                                   bool isSelected,
                                   bool selectedBoxCursorVisible);
    static void drawBoxCloseButton(QPainter &painter,
                                   const BoxViewDTO &box,
                                   const BoxScreenGeometry &geom,
                                   double buttonSize,
                                   double buttonMargin,
                                   double gridGap,
                                   bool isCtrlPressed,
                                   int hoveredBoxId);
    static QColor resolveToolColor(const QString &colorToken, const QColor &fallback);
};
#endif // CANVASPAINTER_H
