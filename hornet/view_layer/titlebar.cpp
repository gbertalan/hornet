#include "view_layer/titlebar.h"
#include "theme.h"
#include <QMouseEvent>
#include <QPalette>
#include <QScreen>
#include <QTimer>

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
        m_grabRatio = static_cast<double>(event->pos().x()) / width();
        m_dragStartPosition = event->globalPos() - window()->frameGeometry().topLeft();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (!window()->isFullScreen()) {
        if (!m_dragging) return;
        window()->move(event->globalPos() - m_dragStartPosition);
    } else if (!m_doubleclicked) {
        m_dragging = false;
        window()->showNormal();
        window()->resize(800, 600);
        window()->move(event->globalPos().x() - static_cast<int>(m_grabRatio * window()->width()),
                       event->globalPos().y() - height() / 2);
        m_dragStartPosition = event->globalPos() - window()->frameGeometry().topLeft();
        m_dragging = true;
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event) {
    m_dragging = false;
    m_doubleclicked = false;
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_doubleclicked = true;
        if (window()->isFullScreen()) {
            window()->showNormal();
        } else {
            window()->showFullScreen();
        }
    }
}
