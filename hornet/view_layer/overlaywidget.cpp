#include "view_layer/overlaywidget.h"
#include "theme.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>

OverlayWidget::OverlayWidget(QWidget* parent) : QWidget(parent), m_fullscreen(false), m_focused(true) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void OverlayWidget::setFullscreen(bool fullscreen) {
    m_fullscreen = fullscreen;
    update();
}

void OverlayWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor cornerColor;
    if (m_fullscreen) {
        cornerColor = QColor(Qt::black);
    } else if (m_focused) {
        cornerColor = Theme::darkGray();
    } else {
        cornerColor = Theme::almostBlack();
    }
    QBrush brush(cornerColor);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);

    int c = 22;

    // top left
    QPainterPath topLeft;
    topLeft.moveTo(0, 0);
    topLeft.lineTo(0, c);
    topLeft.quadTo(0, 0, c, 0);
    topLeft.lineTo(0, 0);
    painter.drawPath(topLeft);

    // top right
    QPainterPath topRight;
    topRight.moveTo(width(), 0);
    topRight.lineTo(width() - c, 0);
    topRight.quadTo(width(), 0, width(), c);
    topRight.lineTo(width(), 0);
    painter.drawPath(topRight);

    // bottom left
    QPainterPath bottomLeft;
    bottomLeft.moveTo(0, height());
    bottomLeft.lineTo(c, height());
    bottomLeft.quadTo(0, height(), 0, height() - c);
    bottomLeft.lineTo(0, height());
    painter.drawPath(bottomLeft);

    // bottom right
    QPainterPath bottomRight;
    bottomRight.moveTo(width(), height());
    bottomRight.lineTo(width(), height() - c);
    bottomRight.quadTo(width(), height(), width() - c, height());
    bottomRight.lineTo(width(), height());
    painter.drawPath(bottomRight);

    // border
    QPen pen(m_focused ? Theme::darkGray() : QColor(Qt::black));
        pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(1, 1, width() - 2, height() - 2, 15, 15);
}

void OverlayWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    raise();
}

void OverlayWidget::setFocused(bool focused) {
    m_focused = focused;
    update();
}
