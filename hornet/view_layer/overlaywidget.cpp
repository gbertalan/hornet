#include "view_layer/overlaywidget.h"
#include "theme.h"
#include <QPainter>
#include <QPen>

OverlayWidget::OverlayWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
}

void OverlayWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Theme::brightAmber());
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(1, 1, width() - 2, height() - 2);
}

void OverlayWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    raise();
}
