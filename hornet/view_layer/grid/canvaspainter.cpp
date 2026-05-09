#include "canvaspainter.h"
#include <cmath>

void CanvasPainter::drawGrid(QPainter &painter, double gridGap, QPoint offset, QSize size)
{
    constexpr double minVisibleGap = 7.18;
    constexpr double maxGap = 325.49;
    const int alpha = static_cast<int>(
        std::clamp((gridGap - minVisibleGap) / (maxGap - minVisibleGap), 0.0, 1.0) * 255);

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(180, 180, 180, alpha), 1));

    const double startX = std::fmod(offset.x(), gridGap);
    const double startY = std::fmod(offset.y(), gridGap);

    const int verticalLines = static_cast<int>(std::ceil((size.width() - startX) / gridGap)) + 1;
    for (int i = 0; i < verticalLines; ++i) {
        const double x = startX + i * gridGap;
        painter.drawLine(QPointF(x, 0), QPointF(x, size.height()));
    }

    const int horizontalLines = static_cast<int>(std::ceil((size.height() - startY) / gridGap)) + 1;
    for (int i = 0; i < horizontalLines; ++i) {
        const double y = startY + i * gridGap;
        painter.drawLine(QPointF(0, y), QPointF(0 + size.width(), y));
    }
}

void CanvasPainter::drawBoxes(QPainter &painter,
                              double gridGap,
                              QPoint offset,
                              const std::vector<BoxModel> &boxes,
                              FontRenderer &fontRenderer)
{
    painter.setRenderHint(QPainter::Antialiasing, true);

    const double edgeThickness = gridGap / 10.0;
    const int headerHeightUnits = 2;
    const float textScale = static_cast<float>(gridGap * 0.8) / fontRenderer.charWidth();
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

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(50, 50, 60));
        painter.drawRect(bodyRect);

        painter.setBrush(QColor(70, 70, 90));
        painter.drawRect(headerRect);

        painter.setPen(QPen(QColor(120, 120, 150), edgeThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(fullRect);

        fontRenderer.drawText(painter,
                              static_cast<float>(screenX + textPadding),
                              static_cast<float>(screenY + textPadding),
                              box.getHeaderText(),
                              QColor(220, 220, 220),
                              textScale);

        for (int i = 0; i < box.getBodyLines().size(); ++i) {
            const float lineY = static_cast<float>(screenY + headerH + textPadding + i * gridGap);
            fontRenderer.drawText(painter,
                                  static_cast<float>(screenX + textPadding),
                                  lineY,
                                  box.getBodyLines()[i],
                                  QColor(180, 180, 180),
                                  textScale);
        }
    }
}
