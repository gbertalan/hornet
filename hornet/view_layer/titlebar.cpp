#include "view_layer/titlebar.h"
#include "view_layer/titlebarbutton.h"

#include "theme.h"
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QScreen>
#include <QTimer>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent), m_dragging(false), m_doubleclicked(false), m_grabRatio(0.0) {
    m_window = qobject_cast<Window*>(parent);
    setFixedHeight(40);
    setAttribute(Qt::WA_TranslucentBackground);

    m_trayButton     = new TitlebarButton(TitlebarButtonType::Tray, this);
    m_minimizeButton = new TitlebarButton(TitlebarButtonType::Minimize, this);
    m_maximizeButton = new TitlebarButton(TitlebarButtonType::Maximize, this);
    m_closeButton    = new TitlebarButton(TitlebarButtonType::Close, this);
    m_closeButton->setHoverColor(Theme::mediumRed());
    m_closeButton->setRightPadding(5);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);
    layout->addStretch();
    layout->addWidget(m_trayButton);
    layout->addWidget(m_minimizeButton);
    layout->addWidget(m_maximizeButton);
    layout->addWidget(m_closeButton);
    layout->setAlignment(m_closeButton, Qt::AlignTop);
    setLayout(layout);

    connect(m_closeButton,    &TitlebarButton::clicked, this, &TitleBar::closeClicked);
    connect(m_minimizeButton, &TitlebarButton::clicked, this, &TitleBar::minimizeClicked);
    connect(m_maximizeButton, &TitlebarButton::clicked, this, &TitleBar::maximizeClicked);
}

void TitleBar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), Theme::darkGrayTranslucent());
    painter.setPen(QPen(QColor(10, 10, 10, 230), 2));
    painter.drawLine(0, height() - 1, width(), height() - 1);
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
