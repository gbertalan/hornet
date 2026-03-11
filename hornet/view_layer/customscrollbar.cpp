#include "view_layer/customscrollbar.h"
#include "theme.h"
#include <QPainter>

CustomScrollBar::CustomScrollBar(Qt::Orientation orientation, QWidget* parent)
    : QScrollBar(orientation, parent), m_hovered(false) {

}

void CustomScrollBar::paintEvent(QPaintEvent* event) {
    QScrollBar::paintEvent(event);
    if (maximum() == 0) return;
    QPainter painter(this);

    if (orientation() == Qt::Vertical) {
        double ratio = static_cast<double>(value()) / maximum();
        int handleHeight = qMax(20, height() * height() / (height() + maximum()));
        int handleY = static_cast<int>(ratio * (height() - handleHeight));

        if (m_hovered) {
            painter.setBrush(Theme::almostWhite());
            painter.setPen(Qt::NoPen);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawRoundedRect(0, handleY, width(), handleHeight, 2, 2);
        } else {
            painter.setPen(QPen(Theme::almostWhite(), 1));
            painter.drawLine(width() - 1, handleY, width() - 1, handleY + handleHeight);
        }
    } else {
        double ratio = static_cast<double>(value()) / maximum();
        int handleWidth = qMax(20, width() * width() / (width() + maximum()));
        int handleX = static_cast<int>(ratio * (width() - handleWidth));

        if (m_hovered) {
            painter.setBrush(Theme::almostWhite());
            painter.setPen(Qt::NoPen);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawRoundedRect(handleX, 0, handleWidth, height()-2, 2, 2);
        } else {
            painter.setPen(QPen(Theme::almostWhite(), 1));
            painter.drawLine(handleX, 1, handleX + handleWidth, 1);
        }
    }
}

void CustomScrollBar::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
}

void CustomScrollBar::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
}
