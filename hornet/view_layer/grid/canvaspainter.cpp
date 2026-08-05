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

    if (gridGap <= minVisibleGap)
        return;

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

BoxResizeEdge CanvasPainter::findResizeEdgeAtPosition(QPoint mousePosition,
                                                      double gridGap,
                                                      QPoint offset,
                                                      const std::vector<BoxViewDTO> &boxes,
                                                      int &outBoxId)
{
    const double margin = gridGap * 0.3;
    for (const BoxViewDTO &box : boxes) {
        const QRectF rect = getBoxScreenRect(box, gridGap, offset);
        const bool inVerticalSpan = mousePosition.y() >= rect.top() - margin
                                    && mousePosition.y() <= rect.bottom() + margin;
        const bool inHorizontalSpan = mousePosition.x() >= rect.left() - margin
                                      && mousePosition.x() <= rect.right() + margin;
        if (!inVerticalSpan || !inHorizontalSpan)
            continue;

        const bool nearLeft = mousePosition.x() <= rect.left() + margin;
        const bool nearRight = mousePosition.x() >= rect.right() - margin;
        const bool nearTop = mousePosition.y() <= rect.top() + margin;
        const bool nearBottom = mousePosition.y() >= rect.bottom() - margin;

        outBoxId = box.id;

        if (nearTop && nearLeft)
            return BoxResizeEdge::TopLeft;
        if (nearTop && nearRight)
            return BoxResizeEdge::TopRight;
        if (nearBottom && nearLeft)
            return BoxResizeEdge::BottomLeft;
        if (nearBottom && nearRight)
            return BoxResizeEdge::BottomRight;
        if (nearTop)
            return BoxResizeEdge::Top;
        if (nearBottom)
            return BoxResizeEdge::Bottom;
        if (nearLeft)
            return BoxResizeEdge::Left;
        if (nearRight)
            return BoxResizeEdge::Right;
    }
    outBoxId = -1;
    return BoxResizeEdge::None;
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
                              FontAtlas &fontAtlas,
                              bool selectedBoxCursorVisible,
                              bool isCtrlPressed,
                              QSize viewportSize)
{
    painter.setRenderHint(QPainter::Antialiasing, true);

    const double edgeThickness = gridGap / 10.0;
    const double halfEdge = edgeThickness / 2.0;

    const int headerHeightUnits = 3;
    const float textScale = static_cast<float>(gridGap * 0.8)
                            / static_cast<float>(fontAtlas.cellHeight());
    const float textHeight = static_cast<float>(fontAtlas.cellHeight()) * textScale;
    const float lineOffset = (static_cast<float>(gridGap) - textHeight) / 2.0f;
    const float textPadding = static_cast<float>(gridGap * 0.2);

    const QRectF viewportRect(0, 0, viewportSize.width(), viewportSize.height());

    for (const BoxViewDTO &box : boxes) {
        // ============================================================
        // SLICE: per-box geometry setup (screen rects for body/header/
        // border/clip, derived from grid position + live drag offset)
        // ============================================================
        const QPoint liveOffset = (box.id == draggedBoxId) ? draggedBoxLiveOffset : QPoint(0, 0);
        const double buttonSize = gridGap * 1.9;
        const double buttonMargin = gridGap * 0.1;
        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset, liveOffset);

        // ============================================================
        // SLICE: visibility culling - skip all further work for boxes
        // entirely outside the visible viewport
        // ============================================================
        if (!fullRect.intersects(viewportRect))
            continue;

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

        const bool isSelected = (box.id == selectedBoxId);
        const bool isHovered = (box.id == hoveredBoxId) && !isSelected;

        // ============================================================
        // SLICE: body / header / border backgrounds
        // ============================================================
        painter.setPen(Qt::NoPen);
        painter.setBrush(isHovered ? Theme::darkerGray() : Theme::almostBlack());
        painter.drawRect(bodyRect);

        // header background:
        QColor headerColor;
        if (isSelected)
            headerColor = Theme::almostBlack();
        else if (isHovered)
            headerColor = Theme::darkerAmber();
        else
            headerColor = Theme::almostBlack();
        painter.setBrush(headerColor);
        painter.drawRect(headerRect);

        painter.setPen(QPen(isSelected ? Theme::almostWhite()
                                       : (isHovered ? Theme::brightAmber() : Theme::darkAmber()),
                            edgeThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(borderRect);

        painter.setClipRect(clipRect);
        painter.setClipping(true);

        // ============================================================
        // SLICE: header text (filename, with ellipsis truncation) + box ID label
        // ============================================================
        float scaleFactor = 2.5f;
        const float buttonAreaWidth = static_cast<float>(buttonSize + buttonMargin);
        const float oneCharWidth = fontAtlas.textWidth(1, textScale) * scaleFactor;
        const float leftMargin = oneCharWidth;
        const float availableWidth = static_cast<float>(screenW) - buttonAreaWidth
                                     - (leftMargin * 2.0f);

        QString displayText = box.headerText;
        float displayTextWidth = fontAtlas.textWidth(box.headerText.length(), textScale)
                                 * scaleFactor;

        if (displayTextWidth > availableWidth) {
            const float ellipsisWidth = fontAtlas.textWidth(3, textScale) * scaleFactor;
            const float maxTextWidth = availableWidth - ellipsisWidth;
            displayText = box.headerText;
            while (displayText.length() > 0) {
                displayTextWidth = fontAtlas.textWidth(displayText.length(), textScale)
                                   * scaleFactor;
                if (displayTextWidth <= maxTextWidth)
                    break;
                displayText = displayText.left(displayText.length() - 1);
            }
            displayText += "...";
        }

        float headerTextX = static_cast<float>(screenX + screenW / 2.0) - displayTextWidth / 2.0f;

        fontRenderer.drawText(painter,
                              headerTextX,
                              static_cast<float>(screenY) + lineOffset,
                              displayText,
                              Theme::darkAmber(),
                              textScale * scaleFactor);
        const QString boxIdLabel = "#" + QString::number(box.id);
        fontRenderer.drawText(painter,
                              static_cast<float>(screenX) + textPadding,
                              static_cast<float>(screenY) + lineOffset,
                              boxIdLabel,
                              Theme::darkAmber(),
                              textScale);

        // ============================================================
        // SLICE: line number gutter setup (width sized to total line
        // count, not just the visible slice, so it doesn't resize on scroll)
        // ============================================================
        const int digits = std::max(1,
                                    static_cast<int>(
                                        QString::number(box.totalBodyLineCount).length()));
        const float gutterWidth = fontAtlas.textWidth(digits + 2, textScale);
        const float gutterX = static_cast<float>(screenX + textPadding);
        const float bodyTextX = gutterX + gutterWidth;

        const QVector<QString> &bodyLines = box.bodyLines;

        // ============================================================
        // SLICE: empty-box case (0 lines) - still show a "line 1" gutter
        // number, its separator segment, and a caret
        // ============================================================
        if (bodyLines.isEmpty()) { // if empty, still draw caret
            const float lineY = static_cast<float>(screenY + headerH) + lineOffset;
            const float textX = bodyTextX + (textPadding * 2);
            const float charWidth = fontAtlas.textWidth(1, textScale);
            if (!isSelected || selectedBoxCursorVisible)
                painter.fillRect(QRectF(textX, lineY + 2, charWidth, static_cast<float>(gridGap)),
                                 Theme::brightAmber());

            const QString lineNumber = QString::number(box.bodyScrollOffset + 1);
            const float numberWidth = fontAtlas.textWidth(lineNumber.length(), textScale);
            const float numberX = gutterX + (gutterWidth - numberWidth) / 2.0f;
            fontRenderer.drawText(painter, numberX, lineY, lineNumber, Theme::darkGray(), textScale);

            painter.save();
            painter.setPen(QPen(Theme::darkGray(), 1));
            painter.drawLine(QPointF(bodyTextX, screenY + headerH + 1),
                             QPointF(bodyTextX, screenY + headerH + gridGap + 1));
            painter.restore();
        }

        // ============================================================
        // SLICE: body text, per-line numbers (with cursor-line highlight),
        // and the caret (blinking if selected, solid otherwise)
        // ============================================================
        for (int i = 0; i < bodyLines.size(); ++i) {
            const float lineY = static_cast<float>(screenY + headerH) + lineOffset
                                + i * static_cast<float>(gridGap);

            const QString lineNumber = QString::number(box.bodyScrollOffset + i + 1);
            const float numberWidth = fontAtlas.textWidth(lineNumber.length(), textScale);
            const float numberX = gutterX + (gutterWidth - numberWidth) / 2.0f;
            const bool isCursorLine = (box.bodyScrollOffset + i == box.cursorY);
            fontRenderer.drawText(painter,
                                  numberX,
                                  lineY,
                                  lineNumber,
                                  isCursorLine ? Theme::brightYellow() : Theme::darkGray(),
                                  textScale);

            const float textX = bodyTextX + (textPadding * 2);
            fontRenderer.drawText(painter, textX, lineY, bodyLines[i], Theme::darkAmber(), textScale);

            // draw cursor in the selected box:
            const int lineIndex = box.bodyScrollOffset + i;
            const bool shouldDrawCursor = (lineIndex == box.cursorY)
                                          && (!isSelected || selectedBoxCursorVisible);
            if (shouldDrawCursor) {
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

        // ============================================================
        // SLICE: gutter separator lines (one short segment per visible
        // text line, not one continuous line)
        // ============================================================
        painter.save();
        painter.setPen(QPen(Theme::darkGray(), 1));
        for (int i = 0; i < bodyLines.size(); ++i) {
            const double lineTop = screenY + headerH + i * gridGap;
            painter.drawLine(QPointF(bodyTextX, lineTop + 1),
                             QPointF(bodyTextX, lineTop + gridGap + 1));
        }
        painter.restore();

        // ============================================================
        // SLICE: close (X) button - hidden for the terminal box, color
        // depends on whether Ctrl is currently held
        // ============================================================
        if (box.contentType != BoxContentType::Terminal) {
            const double margin = buttonMargin;
            const double buttonX = screenX + screenW - buttonSize - margin;
            const double buttonY = screenY + margin;
            const double lineThickness = gridGap * 0.15;
            const QColor closeButtonColor = isCtrlPressed ? Theme::darkAmber() : Theme::darkGray();
            painter.setPen(QPen(closeButtonColor, lineThickness));
            painter.setBrush(Qt::NoBrush);

            const double xPadding = buttonSize * 0.25;
            painter.drawLine(QPointF(buttonX + xPadding, buttonY + xPadding),
                             QPointF(buttonX + buttonSize - xPadding,
                                     buttonY + buttonSize - xPadding));
            painter.drawLine(QPointF(buttonX + buttonSize - xPadding, buttonY + xPadding),
                             QPointF(buttonX + xPadding, buttonY + buttonSize - xPadding));
        }
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

int CanvasPainter::findBoxCloseButtonAtPosition(QPoint mousePosition,
                                                double gridGap,
                                                QPoint offset,
                                                int hoveredBoxId,
                                                const std::vector<BoxViewDTO> &boxes)
{
    if (hoveredBoxId == -1)
        return -1;

    const double buttonSize = gridGap * 1.9;
    const double margin = gridGap * 0.1;
    for (const BoxViewDTO &box : boxes) {
        if (box.id != hoveredBoxId)
            continue;
        if (box.contentType == BoxContentType::Terminal)
            return -1;
        const QRectF rect = getBoxScreenRect(box, gridGap, offset);
        const double buttonX = rect.right() - buttonSize - margin;
        const double buttonY = rect.top() + margin;
        const QRectF buttonRect(buttonX, buttonY, buttonSize, buttonSize);
        if (buttonRect.contains(mousePosition))
            return box.id;
        break;
    }
    return -1;
}
