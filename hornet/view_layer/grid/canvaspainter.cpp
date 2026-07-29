#include "canvaspainter.h"
#include "view_layer/theme.h"
#include <algorithm>
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
    QColor gridColor = Theme::darkerAmber();
    gridColor.setAlpha(alpha);
    painter.setPen(QPen(gridColor, 1));

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

QRectF CanvasPainter::getBoxScreenRect(const BoxViewDTO &box,
                                       double gridGap,
                                       QPoint offset,
                                       QPoint liveOffset)
{
    const double screenX = offset.x() + box.posX * gridGap + liveOffset.x();
    const double screenY = offset.y() + box.posY * gridGap + liveOffset.y();
    const double screenW = box.width * gridGap;
    const double screenH = box.height * gridGap;
    return QRectF(screenX, screenY, screenW, screenH);
}

void CanvasPainter::drawBoxes(QPainter &painter,
                              double gridGap,
                              QPoint offset,
                              const std::vector<BoxViewDTO> &boxes,
                              int hoveredBoxId,
                              int selectedBoxId,
                              int draggedBoxId,
                              QPoint draggedBoxLiveOffset,
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

    for (const BoxViewDTO &box : boxes) {
        const QPoint liveOffset = (box.id == draggedBoxId) ? draggedBoxLiveOffset : QPoint(0, 0);
        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset, liveOffset);
        const double screenX = fullRect.x();
        const double screenY = fullRect.y();
        const double screenW = fullRect.width();
        const double screenH = fullRect.height();
        const double headerH = headerHeightUnits * gridGap;

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

        const bool isHovered = (box.id == hoveredBoxId);
        const bool isSelected = (box.id == selectedBoxId);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Theme::almostBlack());
        painter.drawRect(bodyRect);

        painter.setBrush(isHovered ? Theme::brightAmber() : Theme::darkAmber());
        painter.drawRect(headerRect);

        painter.setPen(
            QPen(isSelected ? Qt::red : (isHovered ? Theme::brightAmber() : Theme::darkAmber()),
                 edgeThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(borderRect);

        painter.setClipRect(clipRect);
        painter.setClipping(true);

        float scaleFactor = 2.5f;
        const float headerTextWidth = fontAtlas.textWidth(box.headerText.length(), textScale)
                                      * scaleFactor;
        const float oneCharWidth = fontAtlas.textWidth(1, textScale) * scaleFactor;
        const float leftMargin = oneCharWidth;
        const float availableWidth = static_cast<float>(screenW) - (leftMargin * 2.0f);

        float headerTextX = 0.0f;
        if (headerTextWidth > availableWidth) {
            headerTextX = static_cast<float>(screenX) + leftMargin;
        } else {
            headerTextX = static_cast<float>(screenX + screenW / 2.0) - headerTextWidth / 2.0f;
        }

        // header text:
        fontRenderer.drawText(painter,
                              headerTextX,
                              static_cast<float>(screenY) + lineOffset,
                              box.headerText,
                              Theme::almostBlack(),
                              textScale * scaleFactor);

        // line number gutter, sized to the box's total line count (not just the visible slice)
        // so it doesn't resize as the box is scrolled:
        const int digits = std::max(1,
                                    static_cast<int>(
                                        QString::number(box.totalBodyLineCount).length()));
        const float gutterWidth = fontAtlas.textWidth(digits + 2, textScale);
        const float gutterX = static_cast<float>(screenX + textPadding);
        const float bodyTextX = gutterX + gutterWidth;

        // body text + line numbers:
        const QVector<QString> &bodyLines = box.bodyLines;
        for (int i = 0; i < bodyLines.size(); ++i) {
            const float lineY = static_cast<float>(screenY + headerH) + lineOffset
                                + i * static_cast<float>(gridGap);

            const QString lineNumber = QString::number(box.bodyScrollOffset + i + 1);
            const float numberWidth = fontAtlas.textWidth(lineNumber.length(), textScale);
            const float numberX = gutterX + (gutterWidth - numberWidth) / 2.0f;
            fontRenderer.drawText(painter, numberX, lineY, lineNumber, Theme::darkGray(), textScale);

            const float textX = bodyTextX + (textPadding * 2);
            fontRenderer.drawText(painter, textX, lineY, bodyLines[i], Theme::darkAmber(), textScale);

            // draw cursor in the selected box:
            const int lineIndex = box.bodyScrollOffset + i;
            if (isSelected && lineIndex == box.cursorY) {
                const float charWidth = fontAtlas.textWidth(1, textScale);
                const float cursorPixelX = textX + fontAtlas.textWidth(box.cursorX, textScale);

                painter.fillRect(QRectF(cursorPixelX,
                                        lineY + 2,
                                        charWidth,
                                        static_cast<float>(gridGap)),
                                 Theme::brightAmber());

                if (box.cursorX < bodyLines[i].length()) {
                    const QString cursorChar = bodyLines[i][box.cursorX];
                    fontRenderer.drawText(painter,
                                          cursorPixelX,
                                          lineY,
                                          cursorChar,
                                          Theme::almostBlack(),
                                          textScale);
                }
            }
        }

        // gutter separator line:
        painter.save();
        painter.setPen(QPen(Theme::darkGray(), 1));
        for (int i = 0; i < bodyLines.size(); ++i) {
            const double lineTop = screenY + headerH + i * gridGap;
            painter.drawLine(QPointF(bodyTextX, lineTop + 1),
                             QPointF(bodyTextX, lineTop + gridGap + 1));
        }
        painter.restore();

        painter.setClipping(false);
    }
}

int CanvasPainter::findBoxAtPosition(QPoint mousePosition,
                                     double gridGap,
                                     QPoint offset,
                                     const std::vector<BoxViewDTO> &boxes)
{
    for (const BoxViewDTO &box : boxes) {
        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset);
        if (fullRect.contains(mousePosition))
            return box.id;
    }
    return -1;
}
