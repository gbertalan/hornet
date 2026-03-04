#include "view_layer/titlebar.h"
#include "theme.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QScreen>
#include <QTimer>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent), m_dragging(false), m_doubleclicked(false) {
    m_window = qobject_cast<Window*>(parent);
    setFixedHeight(40);
    setAttribute(Qt::WA_TranslucentBackground);
}

void TitleBar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    QColor color = Theme::warmGray();
    color.setAlpha(150);
    painter.fillRect(rect(), color);
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
        m_window->restoreWindowedSize();
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
