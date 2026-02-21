#include "registerdisplay.h"

RegisterDisplay::RegisterDisplay(QWidget *parent)
    : QWidget{parent}
    , isHovered(false)
{
    setFixedSize(300, 200);
}

void RegisterDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QPen pen(Qt::green);
    pen.setWidth(2);
    painter.setPen(pen);

    if (isHovered) {
        painter.setBrush(QColor(155, 155, 155));  // White when hovered
    } else {
        painter.setBrush(QColor(55, 55, 55));     // Dark gray normally
    }

    painter.drawRect(0, 0, width()-1, height()-1);

    painter.setPen(QColor(255, 128, 0));

    painter.drawText(10, 25, "EAX: 0x7FFFFFFF");
    painter.drawText(10, 50, "EBX: 0x00000001");
    painter.drawText(10, 75, "ECX: 0xABCD1234");
}

void RegisterDisplay::enterEvent(QEvent *event)
{
    isHovered = true;
    update();
    QWidget::enterEvent(event);
}

void RegisterDisplay::leaveEvent(QEvent *event)
{
    isHovered = false;
    update();
    QWidget::leaveEvent(event);
}
