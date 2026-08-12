#include "mainpopup.h"
#include <QKeyEvent>
#include <QPainter>
#include "theme.h"

MainPopup::MainPopup(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(m_width, m_height);
    setFocusPolicy(Qt::StrongFocus);
    hide();
}

void MainPopup::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::almostBlack());
    painter.setPen(QPen(Theme::darkAmber(), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));
}

void MainPopup::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        emit closeRequested();
    else
        QWidget::keyPressEvent(event);
}
