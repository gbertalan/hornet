#include "canvaspainter.h"
#include "view_layer/theme.h"
#include <cmath>
#include <qdebug.h>

void CanvasPainter::drawGrid(QPainter &painter, double gridGap, QPoint offset, QSize size)
{
    constexpr double minVisibleGap
        = 7.18; // precomputed from the formula baseGap * pow(zoomFactor, zoomLevel - defaultZoom).
    constexpr double maxGap = 48.31;
    const int alpha = static_cast<int>(
        std::clamp((gridGap - minVisibleGap) / (maxGap - minVisibleGap), 0.0, 1.0) * 255);

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(240, 240, 240, alpha), 1));

    const double startX = std::fmod(offset.x(), gridGap);
    const double startY = std::fmod(offset.y(), gridGap);

    const int noOfVerticalLines = static_cast<int>(std::ceil((size.width() - startX) / gridGap))
                                  + 1;
    for (int i = 0; i < noOfVerticalLines; ++i) {
        const double x = startX + i * gridGap;
        painter.drawLine(QPointF(x, 0), QPointF(x, size.height()));
    }

    const int noOfHorizontalLines = static_cast<int>(std::ceil((size.height() - startY) / gridGap))
                                    + 1;
    for (int i = 0; i < noOfHorizontalLines; ++i) {
        const double y = startY + i * gridGap;
        painter.drawLine(QPointF(0, y), QPointF(0 + size.width(), y));
    }
}

void CanvasPainter::drawBoxes(QPainter &painter,
                              double gridGap,
                              QPoint offset,
                              const std::vector<BoxModel> &boxes,
                              FontRenderer &fontRenderer,
                              FontAtlas &fontAtlas)
{
    painter.setRenderHint(QPainter::Antialiasing, true);

    const double edgeThickness = gridGap / 10.0;
    const double halfEdge = edgeThickness / 2.0;

    const int headerHeightUnits = 3;
    const float textScale = static_cast<float>(gridGap * 0.8)
                            / static_cast<float>(fontAtlas.cellHeight());
    const float textHeight = static_cast<float>(fontAtlas.cellHeight()) * textScale;
    const float lineOffset = (static_cast<float>(gridGap) - textHeight) / 2.0f;
    const float textPadding = static_cast<float>(gridGap * 0.1);

    for (const BoxModel &box : boxes) {
        const double screenX = offset.x() + box.getPosX() * gridGap;
        const double screenY = offset.y() + box.getPosY() * gridGap;
        const double screenW = box.getWidth() * gridGap;
        const double screenH = box.getHeight() * gridGap;
        const double headerH = headerHeightUnits * gridGap;

        const QRectF fullRect(screenX, screenY, screenW, screenH);
        const QRectF headerRect(screenX, screenY, screenW, headerH);
        const QRectF bodyRect(screenX, screenY + headerH, screenW, screenH - headerH);
        const QRectF borderRect(screenX + halfEdge,
                                screenY + halfEdge,
                                screenW - edgeThickness,
                                screenH - edgeThickness);
        const QRectF clipRect(screenX + edgeThickness,
                              screenY + edgeThickness,
                              screenW - (edgeThickness * 2),
                              screenH - (edgeThickness * 2));

        painter.setPen(Qt::NoPen);
        painter.setBrush(Theme::almostBlack());
        painter.drawRect(bodyRect);

        painter.setBrush(Theme::darkAmber());
        painter.drawRect(headerRect);

        painter.setPen(QPen(Theme::darkAmber(), edgeThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(borderRect);

        painter.setClipRect(clipRect);
        painter.setClipping(true);

        float scaleFactor = 2.5f;
        const float headerTextWidth = fontAtlas.textWidth(box.getHeaderText().length(), textScale)
                                      * scaleFactor;
        const float headerTextX = static_cast<float>(screenX + screenW / 2.0)
                                  - headerTextWidth / 2.0f;

        // header text:
        fontRenderer.drawText(painter,
                              headerTextX,
                              static_cast<float>(screenY) + lineOffset,
                              box.getHeaderText(),
                              Theme::almostBlack(),
                              textScale * scaleFactor);

        // body text:
        const QVector<QString> &bodyLines = box.getBodyLines();
        for (int i = 0; i < bodyLines.size(); ++i) {
            const float lineY = static_cast<float>(screenY + headerH) + lineOffset
                                + i * static_cast<float>(gridGap);
            fontRenderer.drawText(painter,
                                  static_cast<float>(screenX + textPadding),
                                  lineY,
                                  bodyLines[i],
                                  Theme::darkAmber(),
                                  textScale);
        }

        painter.setClipping(false);
    }
}
