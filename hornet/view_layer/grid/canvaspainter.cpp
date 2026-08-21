#include "canvaspainter.h"
#include "view_layer/theme.h"
#include <algorithm>
#include <cmath>
#include <qdebug.h>

void CanvasPainter::drawGrid(
    QPainter &painter, double gridGap, QPoint offset, QSize size, bool isDraggingGrid)
{
    constexpr double minVisibleGap = 7.18;
    constexpr double maxGap = 48.31;
    if (gridGap <= minVisibleGap)
        return;

    const int alpha = static_cast<int>(
        std::clamp((gridGap - minVisibleGap) / (maxGap - minVisibleGap), 0.0, 1.0) * 255);

    painter.setRenderHint(QPainter::Antialiasing, false);
    QColor gridColor = Theme::darkerAmber();
    gridColor.setAlpha(alpha);
    painter.setPen(QPen(gridColor, 1));

    const int firstCellX = static_cast<int>(std::floor(-offset.x() / gridGap));
    const double startX = offset.x() + firstCellX * gridGap;
    const int noOfVerticalLines = static_cast<int>(std::ceil((size.width() - startX) / gridGap))
                                  + 1;

    for (int i = 0; i < noOfVerticalLines; ++i) {
        const int absoluteCellX = firstCellX + i;
        if (isDraggingGrid && (std::abs(absoluteCellX) % 10 != 0))
            continue;
        const double x = offset.x() + absoluteCellX * gridGap;
        painter.drawLine(QPointF(x, 0), QPointF(x, size.height()));
    }

    const int firstCellY = static_cast<int>(std::floor(-offset.y() / gridGap));
    const double startY = offset.y() + firstCellY * gridGap;
    const int noOfHorizontalLines = static_cast<int>(std::ceil((size.height() - startY) / gridGap))
                                    + 1;

    for (int i = 0; i < noOfHorizontalLines; ++i) {
        const int absoluteCellY = firstCellY + i;
        if (isDraggingGrid && (std::abs(absoluteCellY) % 10 != 0))
            continue;
        const double y = offset.y() + absoluteCellY * gridGap;
        painter.drawLine(QPointF(0, y), QPointF(size.width(), y));
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

QRectF CanvasPainter::getBoxCloseButtonRect(const BoxViewDTO &box, double gridGap, QPoint offset)
{
    const QRectF fullRect = getBoxScreenRect(box, gridGap, offset);
    const double buttonSize = gridGap * 1.9;
    const double buttonMargin = gridGap * 0.1;
    const double buttonX = fullRect.right() - buttonSize - buttonMargin;
    const double buttonY = fullRect.top() + buttonMargin;
    return QRectF(buttonX, buttonY, buttonSize, buttonSize);
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

void CanvasPainter::drawBoxBackgroundAndBorder(QPainter &painter,
                                               const BoxScreenGeometry &geom,
                                               double edgeThickness)
{
    const double halfEdge = edgeThickness / 2.0;
    const QRectF headerRect(geom.screenX, geom.screenY, geom.screenW, geom.headerH);
    const QRectF bodyRect(geom.screenX, geom.screenY, geom.screenW, geom.screenH);
    const QRectF borderRect(geom.screenX + halfEdge,
                            geom.screenY + halfEdge,
                            geom.screenW - edgeThickness,
                            geom.screenH - edgeThickness);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Theme::almostBlack());
    painter.drawRect(bodyRect);

    painter.setBrush(Theme::almostBlack());
    painter.drawRect(headerRect);

    painter.setPen(QPen(Theme::darkAmber(), edgeThickness));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(borderRect);
}

void CanvasPainter::drawBoxHoverSelectBorder(QPainter &painter,
                                             const BoxScreenGeometry &geom,
                                             double gridGap,
                                             double edgeThickness,
                                             bool isSelected,
                                             bool isHovered,
                                             bool isCtrlPressed)
{
    if (!isSelected && !isHovered)
        return;
    if (isCtrlPressed)
        return;

    // alpha scales linearly between these gridGap sizes:
    constexpr double fadeStart = 6.0; // see drawGrid() for gridGap calc value
    constexpr double fadeEnd = 15.0;  // arbitrary, eyeballed it
    const double gapRatio = std::clamp((gridGap - fadeStart) / (fadeEnd - fadeStart), 0.0, 1.0);

    const QRectF rect(geom.screenX - gridGap,
                      geom.screenY - gridGap,
                      geom.screenW + (gridGap * 2.0),
                      geom.screenH + (gridGap * 2.0));

    constexpr double maxOpacity = 0.8; // 1.0=fully transparent
    const int alpha = static_cast<int>((1.0 - maxOpacity * gapRatio) * 255);

    QColor brushColor = isSelected ? Theme::darkAmber() : Theme::almostWhite();
    brushColor.setAlpha(alpha);

    painter.setPen(Theme::darkerAmber());
    painter.setBrush(brushColor);
    painter.drawRect(rect);
}

void CanvasPainter::drawBoxHeaderText(QPainter &painter,
                                      const BoxViewDTO &box,
                                      const BoxScreenGeometry &geom,
                                      FontRenderer &fontRenderer,
                                      FontAtlas &fontAtlas,
                                      float textScale,
                                      float idTopOffset,
                                      float textPadding,
                                      double buttonSize,
                                      double buttonMargin)
{
    float scaleFactor = 2.5f;
    const float buttonAreaWidth = static_cast<float>(buttonSize + buttonMargin);
    const float oneCharWidth = fontAtlas.textWidth(1, textScale) * scaleFactor;
    const float leftMargin = oneCharWidth;
    const float availableWidth = static_cast<float>(geom.screenW) - buttonAreaWidth
                                 - (leftMargin * 2.0f);

    QString displayText = box.headerText;
    float displayTextWidth = fontAtlas.textWidth(box.headerText.length(), textScale) * scaleFactor;

    if (displayTextWidth > availableWidth) {
        const float ellipsisWidth = fontAtlas.textWidth(3, textScale) * scaleFactor;
        const float maxTextWidth = availableWidth - ellipsisWidth;
        displayText = box.headerText;
        while (displayText.length() > 0) {
            displayTextWidth = fontAtlas.textWidth(displayText.length(), textScale) * scaleFactor;
            if (displayTextWidth <= maxTextWidth)
                break;
            displayText = displayText.left(displayText.length() - 1);
        }
        displayText += "...";
    }

    const float headerTextVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                            + fontAtlas.getDescenderPx())
                                         * textScale * scaleFactor;
    const float headerTextY = static_cast<float>(geom.screenY)
                              + static_cast<float>(geom.headerH) / 2.0f
                              - headerTextVisualHeight / 2.0f;

    float headerTextX = static_cast<float>(geom.screenX + geom.screenW / 2.0)
                        - displayTextWidth / 2.0f;

    fontRenderer.drawText(painter,
                          headerTextX,
                          headerTextY,
                          displayText,
                          Theme::darkAmber(),
                          textScale * scaleFactor);
    const QString boxIdLabel = "#" + QString::number(box.id);
    fontRenderer.drawText(painter,
                          static_cast<float>(geom.screenX) + textPadding,
                          static_cast<float>(geom.screenY) + idTopOffset,
                          boxIdLabel,
                          Theme::darkGray(),
                          textScale);
}

QColor CanvasPainter::resolveToolColor(const QString &colorToken, const QColor &fallback)
{
    if (colorToken.isEmpty())
        return fallback;
    if (colorToken.startsWith('#')) {
        const QColor parsed(colorToken);
        return parsed.isValid() ? parsed : fallback;
    }
    bool ok = false;
    const int index = colorToken.toInt(&ok);
    if (!ok)
        return fallback;
    switch (index) {
    case 0:
        return Theme::brightAmber();
    case 1:
        return Theme::darkAmber();
    case 2:
        return Theme::desaturatedTeal();
    case 3:
        return Theme::almostWhite();
    case 4:
        return Theme::darkGray();
    case 5:
        return Theme::almostBlack();
    default:
        return fallback;
    }
}

void CanvasPainter::drawToolScriptPrimitives(QPainter &painter,
                                             const BoxViewDTO &box,
                                             const BoxScreenGeometry &geom,
                                             double gridGap,
                                             FontRenderer &fontRenderer,
                                             FontAtlas &fontAtlas,
                                             float textScale,
                                             bool isCtrlPressed,
                                             int hoveredBoxId,
                                             int hoveredButtonBoxId,
                                             int hoveredButtonIndex,
                                             ToolHoverKind hoveredToolKind)
{
    painter.save();
    painter.setBrush(Qt::NoBrush);
    for (const ToolLineDTO &toolLine : box.toolScript.lines) {
        const double lx1 = geom.screenX + toolLine.x1 * gridGap;
        const double ly1 = geom.screenY + geom.headerH + toolLine.y1 * gridGap;
        const double lx2 = geom.screenX + toolLine.x2 * gridGap;
        const double ly2 = geom.screenY + geom.headerH + toolLine.y2 * gridGap;
        const double thickness = std::max(1.0, gridGap * 0.1 * toolLine.thicknessMultiplier);
        painter.setPen(QPen(resolveToolColor(toolLine.colorToken, Theme::brightAmber()), thickness));
        painter.drawLine(QPointF(lx1, ly1), QPointF(lx2, ly2));
    }
    for (const ToolRectDTO &toolRect : box.toolScript.rects) {
        const double rx = geom.screenX + toolRect.x * gridGap;
        const double ry = geom.screenY + geom.headerH + toolRect.y * gridGap;
        const double rw = toolRect.width * gridGap;
        const double rh = toolRect.height * gridGap;
        const QColor color = resolveToolColor(toolRect.colorToken, Theme::brightAmber());
        if (toolRect.filled) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
        } else {
            const double thickness = std::max(1.0, gridGap * 0.1 * toolRect.thicknessMultiplier);
            painter.setPen(QPen(color, thickness));
            painter.setBrush(Qt::NoBrush);
        }
        painter.drawRect(QRectF(rx, ry, rw, rh));
    }
    for (const ToolCircleDTO &toolCircle : box.toolScript.circles) {
        const double cx = geom.screenX + toolCircle.x * gridGap;
        const double cy = geom.screenY + geom.headerH + toolCircle.y * gridGap;
        const double r = toolCircle.radius * gridGap;
        const QColor color = resolveToolColor(toolCircle.colorToken, Theme::brightAmber());
        if (toolCircle.filled) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(color);
        } else {
            const double thickness = std::max(1.0, gridGap * 0.1 * toolCircle.thicknessMultiplier);
            painter.setPen(QPen(color, thickness));
            painter.setBrush(Qt::NoBrush);
        }
        painter.drawEllipse(QPointF(cx, cy), r, r);
    }
    painter.restore();
    for (const ToolTextDTO &toolText : box.toolScript.texts) {
        const double tx = geom.screenX + toolText.x * gridGap;
        const double ty = geom.screenY + geom.headerH + toolText.y * gridGap;
        fontRenderer.drawText(painter,
                              static_cast<float>(tx),
                              static_cast<float>(ty),
                              toolText.text,
                              resolveToolColor(toolText.colorToken, Theme::brightAmber()),
                              textScale * static_cast<float>(toolText.fontSizeMultiplier));
    }
    drawToolButtons(painter,
                    box,
                    geom,
                    gridGap,
                    fontRenderer,
                    fontAtlas,
                    textScale,
                    isCtrlPressed,
                    hoveredBoxId,
                    hoveredButtonBoxId,
                    hoveredButtonIndex,
                    hoveredToolKind);
    drawToolTextFields(painter,
                       box,
                       geom,
                       gridGap,
                       fontRenderer,
                       fontAtlas,
                       textScale,
                       isCtrlPressed,
                       hoveredBoxId,
                       hoveredButtonBoxId,
                       hoveredButtonIndex,
                       hoveredToolKind);
    drawToolDropdowns(painter,
                      box,
                      geom,
                      gridGap,
                      fontRenderer,
                      fontAtlas,
                      textScale,
                      isCtrlPressed,
                      hoveredBoxId,
                      hoveredButtonBoxId,
                      hoveredButtonIndex,
                      hoveredToolKind);
}

void CanvasPainter::drawToolButtons(QPainter &painter,
                                    const BoxViewDTO &box,
                                    const BoxScreenGeometry &geom,
                                    double gridGap,
                                    FontRenderer &fontRenderer,
                                    FontAtlas &fontAtlas,
                                    float textScale,
                                    bool isCtrlPressed,
                                    int hoveredBoxId,
                                    int hoveredButtonBoxId,
                                    int hoveredButtonIndex,
                                    ToolHoverKind hoveredToolKind)
{
    if (box.toolScript.buttons.empty())
        return;

    painter.save();

    for (int i = 0; i < static_cast<int>(box.toolScript.buttons.size()); ++i) {
        const ToolButtonDTO &toolButton = box.toolScript.buttons.at(i);
        const bool isActive = isCtrlPressed && box.id == hoveredBoxId;
        const bool isHovered = isActive && hoveredToolKind == ToolHoverKind::Button
                               && box.id == hoveredButtonBoxId && i == hoveredButtonIndex;

        const double bx = geom.screenX + toolButton.x * gridGap;
        const double by = geom.screenY + geom.headerH + toolButton.y * gridGap;
        const double bw = toolButton.width * gridGap;
        const double bh = toolButton.height * gridGap;
        const QRectF buttonRect(bx, by, bw, bh);

        const QColor themeColor = resolveToolColor(toolButton.colorToken, Theme::brightAmber());
        const double borderThickness = std::max(1.0, gridGap * 0.08);

        QColor backgroundColor;
        QColor borderColor;
        QColor textColor;
        QPixmap glyphBackground;

        if (!isActive) {
            backgroundColor = QColor("#1a1a1a");
            borderColor = Theme::darkGray();
            textColor = Theme::darkGray();
        } else if (isHovered) {
            backgroundColor = QColor("#262626");
            borderColor = themeColor;
            textColor = themeColor;
            glyphBackground = QPixmap(":/icons/titlebar_button_glow.png");
        } else {
            backgroundColor = QColor("#1a1a1a");
            borderColor = QColor("#4e4c4a");
            textColor = themeColor;
            glyphBackground = QPixmap(":/icons/titlebar_button_background.png");
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(backgroundColor);
        painter.drawRect(buttonRect);

        painter.setPen(QPen(borderColor, borderThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(buttonRect);

        QString displayLabel = toolButton.label;
        float labelWidth = fontAtlas.textWidth(displayLabel.length(), textScale);
        if (labelWidth > static_cast<float>(bw)) {
            const float ellipsisWidth = fontAtlas.textWidth(3, textScale);
            const float maxTextWidth = static_cast<float>(bw) - ellipsisWidth;
            while (!displayLabel.isEmpty()) {
                labelWidth = fontAtlas.textWidth(displayLabel.length(), textScale);
                if (labelWidth <= maxTextWidth)
                    break;
                displayLabel.chop(1);
            }
            displayLabel += "...";
            labelWidth = fontAtlas.textWidth(displayLabel.length(), textScale);
        }

        const float charWidth = fontAtlas.textWidth(1, textScale);
        const float textVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                          + fontAtlas.getDescenderPx())
                                       * textScale;
        const float textX = static_cast<float>(bx + bw / 2.0) - labelWidth / 2.0f;
        const float textY = static_cast<float>(by + bh / 2.0) - textVisualHeight / 2.0f;

        if (!glyphBackground.isNull()) {
            float glyphX = textX;
            for (int charIndex = 0; charIndex < displayLabel.length(); ++charIndex) {
                painter.drawPixmap(QRectF(glyphX, textY, charWidth, textVisualHeight).toRect(),
                                   glyphBackground);
                glyphX += charWidth;
            }
        }

        fontRenderer.drawText(painter, textX, textY, displayLabel, textColor, textScale);
    }

    painter.restore();
}

void CanvasPainter::drawToolTextFields(QPainter &painter,
                                       const BoxViewDTO &box,
                                       const BoxScreenGeometry &geom,
                                       double gridGap,
                                       FontRenderer &fontRenderer,
                                       FontAtlas &fontAtlas,
                                       float textScale,
                                       bool isCtrlPressed,
                                       int hoveredBoxId,
                                       int hoveredButtonBoxId,
                                       int hoveredButtonIndex,
                                       ToolHoverKind hoveredToolKind)
{
    if (box.toolScript.textFields.empty())
        return;

    painter.save();

    for (int i = 0; i < static_cast<int>(box.toolScript.textFields.size()); ++i) {
        const ToolTextFieldDTO &toolTextField = box.toolScript.textFields.at(i);
        const bool isActive = isCtrlPressed && box.id == hoveredBoxId;
        const bool isHovered = isActive && hoveredToolKind == ToolHoverKind::TextField
                               && box.id == hoveredButtonBoxId && i == hoveredButtonIndex;

        const double fx = geom.screenX + toolTextField.x * gridGap;
        const double fy = geom.screenY + geom.headerH + toolTextField.y * gridGap;
        const double fw = toolTextField.width * gridGap;
        const double fh = toolTextField.height * gridGap;
        const QRectF fieldRect(fx, fy, fw, fh);

        const double borderThickness = std::max(1.0, gridGap * 0.08);

        QColor backgroundColor;
        QColor borderColor;
        QColor textColor;

        if (!isActive) {
            backgroundColor = QColor("#1a1a1a");
            borderColor = Theme::darkGray();
            textColor = Theme::darkGray();
        } else if (isHovered) {
            backgroundColor = QColor("#262626");
            borderColor = Theme::brightAmber();
            textColor = Theme::almostWhite();
        } else {
            backgroundColor = QColor("#1a1a1a");
            borderColor = QColor("#4e4c4a");
            textColor = Theme::almostWhite();
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(backgroundColor);
        painter.drawRect(fieldRect);

        painter.setPen(QPen(borderColor, borderThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(fieldRect);

        const QString displayText = toolTextField.value.isEmpty() ? toolTextField.name
                                                                  : toolTextField.value;
        const float textVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                          + fontAtlas.getDescenderPx())
                                       * textScale;
        const float textX = static_cast<float>(fx) + static_cast<float>(borderThickness) * 2.0f;
        const float textY = static_cast<float>(fy + fh / 2.0) - textVisualHeight / 2.0f;

        fontRenderer.drawText(painter, textX, textY, displayText, textColor, textScale);
    }

    painter.restore();
}

void CanvasPainter::drawToolDropdowns(QPainter &painter,
                                      const BoxViewDTO &box,
                                      const BoxScreenGeometry &geom,
                                      double gridGap,
                                      FontRenderer &fontRenderer,
                                      FontAtlas &fontAtlas,
                                      float textScale,
                                      bool isCtrlPressed,
                                      int hoveredBoxId,
                                      int hoveredButtonBoxId,
                                      int hoveredButtonIndex,
                                      ToolHoverKind hoveredToolKind)
{
    if (box.toolScript.dropdowns.empty())
        return;

    painter.save();

    for (int i = 0; i < static_cast<int>(box.toolScript.dropdowns.size()); ++i) {
        const ToolDropdownDTO &toolDropdown = box.toolScript.dropdowns.at(i);
        const bool isActive = isCtrlPressed && box.id == hoveredBoxId;
        const bool isHovered = isActive && hoveredToolKind == ToolHoverKind::Dropdown
                               && box.id == hoveredButtonBoxId && i == hoveredButtonIndex;

        const double dx = geom.screenX + toolDropdown.x * gridGap;
        const double dy = geom.screenY + geom.headerH + toolDropdown.y * gridGap;
        const double dw = toolDropdown.width * gridGap;
        const double dh = toolDropdown.height * gridGap;
        const QRectF dropdownRect(dx, dy, dw, dh);

        const double borderThickness = std::max(1.0, gridGap * 0.08);

        QColor backgroundColor;
        QColor borderColor;
        QColor textColor;

        if (!isActive) {
            backgroundColor = QColor("#1a1a1a");
            borderColor = Theme::darkGray();
            textColor = Theme::darkGray();
        } else if (isHovered) {
            backgroundColor = QColor("#262626");
            borderColor = Theme::brightAmber();
            textColor = Theme::almostWhite();
        } else {
            backgroundColor = QColor("#1a1a1a");
            borderColor = QColor("#4e4c4a");
            textColor = Theme::almostWhite();
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(backgroundColor);
        painter.drawRect(dropdownRect);

        painter.setPen(QPen(borderColor, borderThickness));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(dropdownRect);

        const QString displayText = toolDropdown.value;
        const float textVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                          + fontAtlas.getDescenderPx())
                                       * textScale;
        const float textX = static_cast<float>(dx) + static_cast<float>(borderThickness) * 2.0f;
        const float textY = static_cast<float>(dy + dh / 2.0) - textVisualHeight / 2.0f;
        fontRenderer.drawText(painter, textX, textY, displayText, textColor, textScale);

        const double triangleSize = dh * 0.28;
        const double triangleCenterX = dx + dw - static_cast<double>(borderThickness) * 2.0
                                       - triangleSize / 2.0;
        const double triangleCenterY = dy + dh / 2.0;
        QPolygonF triangle;
        triangle << QPointF(triangleCenterX - triangleSize / 2.0,
                            triangleCenterY - triangleSize / 3.0)
                 << QPointF(triangleCenterX + triangleSize / 2.0,
                            triangleCenterY - triangleSize / 3.0)
                 << QPointF(triangleCenterX, triangleCenterY + triangleSize / 2.0);
        painter.setPen(Qt::NoPen);
        painter.setBrush(textColor);
        painter.drawPolygon(triangle);
    }

    painter.restore();
}

void CanvasPainter::drawBoxTextContent(QPainter &painter,
                                       const BoxViewDTO &box,
                                       const BoxScreenGeometry &geom,
                                       FontRenderer &fontRenderer,
                                       FontAtlas &fontAtlas,
                                       double gridGap,
                                       float textScale,
                                       float lineOffset,
                                       float textPadding,
                                       bool isSelected,
                                       bool selectedBoxCursorVisible)
{
    const int digits = std::max(1,
                                static_cast<int>(QString::number(box.totalBodyLineCount).length()));
    const float gutterWidth = fontAtlas.textWidth(digits + 2, textScale);
    const float gutterX = static_cast<float>(geom.screenX + textPadding);
    const float bodyTextX = gutterX + gutterWidth;
    const float textVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                      + fontAtlas.getDescenderPx())
                                   * textScale;

    const QVector<QString> &bodyLines = box.bodyLines;

    drawBoxMarks(painter, box, geom, gridGap);
    drawBoxSelection(painter, box, geom, fontAtlas, gridGap, textScale, bodyTextX, textPadding);

    if (bodyLines.isEmpty()) { // if empty, still draw caret
        const float rowTop = static_cast<float>(geom.screenY + geom.headerH);
        const float lineY = rowTop + static_cast<float>(gridGap) / 2.0f - textVisualHeight / 2.0f;
        const float textX = bodyTextX + (textPadding * 2);
        const float charWidth = fontAtlas.textWidth(1, textScale);
        if (!isSelected || selectedBoxCursorVisible)
            painter.fillRect(QRectF(textX, rowTop, charWidth, static_cast<float>(gridGap)),
                             Theme::brightAmber());

        const QString lineNumber = QString::number(box.bodyScrollOffset + 1);
        const float numberWidth = fontAtlas.textWidth(lineNumber.length(), textScale);
        const float numberX = gutterX + (gutterWidth - numberWidth) / 2.0f;
        fontRenderer.drawText(painter, numberX, lineY, lineNumber, Theme::darkGray(), textScale);

        painter.save();
        painter.setPen(QPen(Theme::darkGray(), 1));
        painter.drawLine(QPointF(bodyTextX, geom.screenY + geom.headerH + 1),
                         QPointF(bodyTextX, geom.screenY + geom.headerH + gridGap + 1));
        painter.restore();
    }

    for (int i = 0; i < bodyLines.size(); ++i) {
        const float rowTop = static_cast<float>(geom.screenY + geom.headerH)
                             + i * static_cast<float>(gridGap);
        const float lineY = rowTop + static_cast<float>(gridGap) / 2.0f - textVisualHeight / 2.0f;

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

            painter.fillRect(QRectF(cursorPixelX, rowTop, charWidth, static_cast<float>(gridGap)),
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

    painter.save();
    painter.setPen(QPen(Theme::darkGray(), 1));
    for (int i = 0; i < bodyLines.size(); ++i) {
        const double lineTop = geom.screenY + geom.headerH + i * gridGap;
        painter.drawLine(QPointF(bodyTextX, lineTop + 1), QPointF(bodyTextX, lineTop + gridGap + 1));
    }
    painter.restore();
}

void CanvasPainter::drawBoxSelection(QPainter &painter,
                                     const BoxViewDTO &box,
                                     const BoxScreenGeometry &geom,
                                     FontAtlas &fontAtlas,
                                     double gridGap,
                                     float textScale,
                                     float bodyTextX,
                                     float textPadding)
{
    if (!box.hasSelection)
        return;

    const int startRow = std::min(box.selectionAnchorY, box.selectionExtentY);
    const int endRow = std::max(box.selectionAnchorY, box.selectionExtentY);
    const float charWidth = fontAtlas.textWidth(1, textScale);
    const float textX = bodyTextX + (textPadding * 2);

    for (int i = 0; i < box.bodyLines.size(); ++i) {
        const int lineIndex = box.bodyScrollOffset + i;
        if (lineIndex < startRow || lineIndex > endRow)
            continue;

        int startCol, endCol;
        if (startRow == endRow) {
            startCol = std::min(box.selectionAnchorX, box.selectionExtentX);
            endCol = std::max(box.selectionAnchorX, box.selectionExtentX);
        } else if (lineIndex == startRow) {
            startCol = (box.selectionAnchorY == startRow) ? box.selectionAnchorX
                                                          : box.selectionExtentX;
            endCol = box.bodyLines[i].length();
        } else if (lineIndex == endRow) {
            startCol = 0;
            endCol = (box.selectionAnchorY == endRow) ? box.selectionAnchorX : box.selectionExtentX;
        } else {
            startCol = 0;
            endCol = box.bodyLines[i].length();
        }

        const float rowTop = static_cast<float>(geom.screenY + geom.headerH)
                             + i * static_cast<float>(gridGap);
        const float startX = textX + startCol * charWidth;
        const float endX = textX + endCol * charWidth;
        const float width = endX - startX;
        if (width > 0)
            painter.fillRect(QRectF(startX, rowTop, width, static_cast<float>(gridGap)),
                             QColor(100, 149, 237, 100));
    }
}

void CanvasPainter::drawBoxMarks(QPainter &painter,
                                 const BoxViewDTO &box,
                                 const BoxScreenGeometry &geom,
                                 double gridGap)
{
    if (box.marks.isEmpty())
        return;

    static const QColor defaultMarkColor(178, 34, 34, 70);

    for (int i = 0; i < box.bodyLines.size(); ++i) {
        const int lineIndex = box.bodyScrollOffset + i;
        const MarkRange *matchedMark = nullptr;
        for (const MarkRange &mark : box.marks) {
            if (lineIndex >= mark.startLine && lineIndex <= mark.endLine) {
                matchedMark = &mark;
                break;
            }
        }
        if (!matchedMark)
            continue;

        QColor markColor = matchedMark->colorToken.isEmpty()
                               ? defaultMarkColor
                               : resolveToolColor(matchedMark->colorToken, defaultMarkColor);
        markColor.setAlpha(70);

        const float rowTop = static_cast<float>(geom.screenY + geom.headerH)
                             + i * static_cast<float>(gridGap);
        painter.fillRect(QRectF(geom.screenX, rowTop, geom.screenW, static_cast<float>(gridGap)),
                         markColor);
    }
}

void CanvasPainter::drawBoxCloseButton(QPainter &painter,
                                       const BoxViewDTO &box,
                                       const BoxScreenGeometry &geom,
                                       double buttonSize,
                                       double buttonMargin,
                                       double gridGap,
                                       bool isCtrlPressed,
                                       int hoveredBoxId)
{
    if (box.contentType == BoxContentType::Terminal || !isCtrlPressed || box.id != hoveredBoxId)
        return;

    const double buttonX = geom.screenX + geom.screenW - buttonSize - buttonMargin;
    const double buttonY = geom.screenY + buttonMargin;
    const double lineThickness = gridGap * 0.15;
    painter.setPen(QPen(Theme::darkAmber(), lineThickness));
    painter.setBrush(Qt::NoBrush);

    const double xPadding = buttonSize * 0.25;
    painter.drawLine(QPointF(buttonX + xPadding, buttonY + xPadding),
                     QPointF(buttonX + buttonSize - xPadding, buttonY + buttonSize - xPadding));
    painter.drawLine(QPointF(buttonX + buttonSize - xPadding, buttonY + xPadding),
                     QPointF(buttonX + xPadding, buttonY + buttonSize - xPadding));
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
                              int hoveredButtonBoxId,
                              int hoveredButtonIndex,
                              ToolHoverKind hoveredToolKind,
                              QSize viewportSize)
{
    painter.setRenderHint(QPainter::Antialiasing, true);

    const double edgeThickness = gridGap / 10.0;
    const int headerHeightUnits = 3;
    const float textScale = static_cast<float>(gridGap * 0.8)
                            / static_cast<float>(fontAtlas.cellHeight());

    const float idTextVisualHeight = static_cast<float>(fontAtlas.getAscenderPx()
                                                        + fontAtlas.getDescenderPx())
                                     * textScale;
    const float idTopOffset = (static_cast<float>(gridGap) - idTextVisualHeight) / 2.0f;
    const float textPadding = static_cast<float>(gridGap * 0.2);

    const QRectF viewportRect(0, 0, viewportSize.width(), viewportSize.height());

    for (const BoxViewDTO &box : boxes) {
        const QPoint liveOffset = (box.id == draggedBoxId) ? draggedBoxLiveOffset : QPoint(0, 0);
        const double buttonSize = gridGap * 1.9;
        const double buttonMargin = gridGap * 0.1;
        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset, liveOffset);

        if (!fullRect.intersects(viewportRect))
            continue;

        const BoxScreenGeometry geom{fullRect.x(),
                                     fullRect.y(),
                                     fullRect.width(),
                                     fullRect.height(),
                                     headerHeightUnits * gridGap};

        const QRectF clipRect(geom.screenX + edgeThickness,
                              geom.screenY + edgeThickness,
                              geom.screenW - (edgeThickness * 2),
                              geom.screenH - (edgeThickness * 2));

        const bool isSelected = (box.id == selectedBoxId);
        const bool isHovered = (box.id == hoveredBoxId) && !isSelected;
        const bool isDragged = (box.id == draggedBoxId);

        if (!isDragged) {
            CanvasPainter::drawBoxHoverSelectBorder(painter,
                                                    geom,
                                                    gridGap,
                                                    edgeThickness,
                                                    isSelected,
                                                    isHovered,
                                                    isCtrlPressed);
        }
        drawBoxBackgroundAndBorder(painter, geom, edgeThickness);

        painter.setClipRect(clipRect);
        painter.setClipping(true);

        drawBoxHeaderText(painter,
                          box,
                          geom,
                          fontRenderer,
                          fontAtlas,
                          textScale,
                          idTopOffset,
                          textPadding,
                          buttonSize,
                          buttonMargin);

        if (box.contentType == BoxContentType::Tool) {
            drawToolScriptPrimitives(painter,
                                     box,
                                     geom,
                                     gridGap,
                                     fontRenderer,
                                     fontAtlas,
                                     textScale,
                                     isCtrlPressed,
                                     hoveredBoxId,
                                     hoveredButtonBoxId,
                                     hoveredButtonIndex,
                                     hoveredToolKind);
        } else if (box.contentType == BoxContentType::PlainText
                   || box.contentType == BoxContentType::List
                   || box.contentType == BoxContentType::Terminal
                   || box.contentType == BoxContentType::Unknown) {
            drawBoxTextContent(painter,
                               box,
                               geom,
                               fontRenderer,
                               fontAtlas,
                               gridGap,
                               textScale,
                               idTopOffset,
                               textPadding,
                               isSelected,
                               selectedBoxCursorVisible);
        }

        drawBoxCloseButton(painter,
                           box,
                           geom,
                           buttonSize,
                           buttonMargin,
                           gridGap,
                           isCtrlPressed,
                           hoveredBoxId);

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

int CanvasPainter::findToolButtonAtPosition(QPoint mousePosition,
                                            double gridGap,
                                            QPoint offset,
                                            int hoveredBoxId,
                                            const std::vector<BoxViewDTO> &boxes,
                                            int &outButtonIndex)
{
    outButtonIndex = -1;
    if (hoveredBoxId == -1)
        return -1;

    constexpr int headerHeightUnits = 3;
    for (const BoxViewDTO &box : boxes) {
        if (box.id != hoveredBoxId)
            continue;
        if (box.toolScript.buttons.empty())
            return -1;

        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset);
        const double headerH = headerHeightUnits * gridGap;
        for (int i = 0; i < static_cast<int>(box.toolScript.buttons.size()); ++i) {
            const ToolButtonDTO &toolButton = box.toolScript.buttons.at(i);
            const double bx = fullRect.x() + toolButton.x * gridGap;
            const double by = fullRect.y() + headerH + toolButton.y * gridGap;
            const double bw = toolButton.width * gridGap;
            const double bh = toolButton.height * gridGap;
            if (QRectF(bx, by, bw, bh).contains(mousePosition)) {
                outButtonIndex = i;
                return box.id;
            }
        }
        return -1;
    }
    return -1;
}

int CanvasPainter::findToolTextFieldAtPosition(QPoint mousePosition,
                                               double gridGap,
                                               QPoint offset,
                                               int hoveredBoxId,
                                               const std::vector<BoxViewDTO> &boxes,
                                               int &outFieldIndex)
{
    outFieldIndex = -1;
    if (hoveredBoxId == -1)
        return -1;

    constexpr int headerHeightUnits = 3;
    for (const BoxViewDTO &box : boxes) {
        if (box.id != hoveredBoxId)
            continue;
        if (box.toolScript.textFields.empty())
            return -1;

        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset);
        const double headerH = headerHeightUnits * gridGap;
        for (int i = 0; i < static_cast<int>(box.toolScript.textFields.size()); ++i) {
            const ToolTextFieldDTO &toolTextField = box.toolScript.textFields.at(i);
            const double fx = fullRect.x() + toolTextField.x * gridGap;
            const double fy = fullRect.y() + headerH + toolTextField.y * gridGap;
            const double fw = toolTextField.width * gridGap;
            const double fh = toolTextField.height * gridGap;
            if (QRectF(fx, fy, fw, fh).contains(mousePosition)) {
                outFieldIndex = i;
                return box.id;
            }
        }
        return -1;
    }
    return -1;
}

int CanvasPainter::findToolDropdownAtPosition(QPoint mousePosition,
                                              double gridGap,
                                              QPoint offset,
                                              int hoveredBoxId,
                                              const std::vector<BoxViewDTO> &boxes,
                                              int &outDropdownIndex)
{
    outDropdownIndex = -1;
    if (hoveredBoxId == -1)
        return -1;

    constexpr int headerHeightUnits = 3;
    for (const BoxViewDTO &box : boxes) {
        if (box.id != hoveredBoxId)
            continue;
        if (box.toolScript.dropdowns.empty())
            return -1;

        const QRectF fullRect = getBoxScreenRect(box, gridGap, offset);
        const double headerH = headerHeightUnits * gridGap;
        for (int i = 0; i < static_cast<int>(box.toolScript.dropdowns.size()); ++i) {
            const ToolDropdownDTO &toolDropdown = box.toolScript.dropdowns.at(i);
            const double dx = fullRect.x() + toolDropdown.x * gridGap;
            const double dy = fullRect.y() + headerH + toolDropdown.y * gridGap;
            const double dw = toolDropdown.width * gridGap;
            const double dh = toolDropdown.height * gridGap;
            if (QRectF(dx, dy, dw, dh).contains(mousePosition)) {
                outDropdownIndex = i;
                return box.id;
            }
        }
        return -1;
    }
    return -1;
}
