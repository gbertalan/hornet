#include "view_layer/titlebar.h"
#include "theme.h"
#include <QMouseEvent>
#include <QPalette>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent), m_dragging(false) {
    setFixedHeight(40);
    QPalette palette;
    palette.setColor(QPalette::Window, Theme::warmGray());
    setPalette(palette);
    setAutoFillBackground(true);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPosition = event->globalPos() - window()->frameGeometry().topLeft();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        window()->move(event->globalPos() - m_dragStartPosition);
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
}
