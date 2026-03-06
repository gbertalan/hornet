#include "view_layer/window.h"
#include "view_layer/titlebar.h"
#include "view_layer/resizehandle.h"
#include "shared/dto/numberdto.h"
#include "theme.h"
#include <QEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPalette>
#include <QVBoxLayout>
#include <QBitmap>
#include <QPainter>
#include "view_layer/lowerwidget.h"
#include "view_layer/overlaywidget.h"

static constexpr int HANDLE_THICKNESS = 6;
static constexpr int CORNER_SIZE = 12;

Window::Window(const WindowDTO& initialState, QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(300, 200);
    m_windowedWidth = initialState.width;
    m_windowedHeight = initialState.height;
    resize(initialState.width, initialState.height);
    m_windowedX = initialState.x;
    m_windowedY = initialState.y;
    move(initialState.x, initialState.y);

    QPalette palette;
    palette.setColor(QPalette::Window, Theme::almostBlack());
    setPalette(palette);
    setAutoFillBackground(true);

    m_lowerWidget = new LowerWidget(this);
    m_lowerWidget->setGeometry(0, 0, width(), height());

    m_titleBar = new TitleBar(this);
    m_titleBar->raise();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_titleBar);
    layout->addStretch();
    setLayout(layout);

    m_overlayWidget = new OverlayWidget(this);
    m_overlayWidget->setGeometry(0, 0, width(), height());
    m_overlayWidget->raise();

    setupResizeHandles();
    positionResizeHandles();

    if (initialState.isFullscreen)
        showFullScreen();

    connect(m_titleBar, &TitleBar::closeClicked,    this, &Window::closeClicked);
    connect(m_titleBar, &TitleBar::minimizeClicked, this, [this]() {
        showMinimized();
    });
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this]() {
        if (isFullScreen()) {
            showNormal();
            restoreWindowedSize();
            restoreWindowedLocation();
        } else {
            showFullScreen();
        }
    });
}

void Window::displayNumber(const NumberDTO& dto) {
}

void Window::setupResizeHandles() {
    m_handleLeft        = new ResizeHandle(ResizeEdge::Left, this);
    m_handleRight       = new ResizeHandle(ResizeEdge::Right, this);
    m_handleTop         = new ResizeHandle(ResizeEdge::Top, this);
    m_handleBottom      = new ResizeHandle(ResizeEdge::Bottom, this);
    m_handleTopLeft     = new ResizeHandle(ResizeEdge::TopLeft, this);
    m_handleTopRight    = new ResizeHandle(ResizeEdge::TopRight, this);
    m_handleBottomLeft  = new ResizeHandle(ResizeEdge::BottomLeft, this);
    m_handleBottomRight = new ResizeHandle(ResizeEdge::BottomRight, this);
}

void Window::positionResizeHandles() {
    int w = width();
    int h = height();
    int t = HANDLE_THICKNESS;
    int c = CORNER_SIZE;

    m_handleTop->setGeometry(c, 0, w - 2 * c, t);
    m_handleBottom->setGeometry(c, h - t, w - 2 * c, t);
    m_handleLeft->setGeometry(0, c, t, h - 2 * c);
    m_handleRight->setGeometry(w - t, c, t, h - 2 * c);

    m_handleTopLeft->setGeometry(0, 0, c, c);
    m_handleTopRight->setGeometry(w - c, 0, c, c);
    m_handleBottomLeft->setGeometry(0, h - c, c, c);
    m_handleBottomRight->setGeometry(w - c, h - c, c, c);
}

void Window::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_lowerWidget->setGeometry(0, 0, width(), height());
    m_overlayWidget->setGeometry(0, 0, width(), height());
    positionResizeHandles();
    if(!isFullScreen()){
        m_windowedWidth = width();
        m_windowedHeight = height();

        WindowDTO dto;
        dto.x = x();
        dto.y = y();
        dto.width = width();
        dto.height = height();
        dto.isFullscreen = false;
        emit windowStateChanged(dto);

        QBitmap maskBitmap(size());
        maskBitmap.fill(Qt::color0);
        QPainter maskPainter(&maskBitmap);
        maskPainter.setBrush(Qt::color1);
        maskPainter.setPen(Qt::color1);
        maskPainter.drawRoundedRect(rect(), 17, 17);

        setMask(maskBitmap);
    } else {
        clearMask();
    }
}

void Window::moveEvent(QMoveEvent* event) {
    QWidget::moveEvent(event);
    if (!isFullScreen()){
        m_windowedX = x();
        m_windowedY = y();

        WindowDTO dto;
        dto.x = x();
        dto.y = y();
        dto.width = width();
        dto.height = height();
        dto.isFullscreen = false;
        emit windowStateChanged(dto);
    }
}

void Window::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        bool fullscreen = isFullScreen();
        m_overlayWidget->setFullscreen(fullscreen);
        m_titleBar->setFullscreen(fullscreen);

        m_handleLeft->setVisible(!fullscreen);
        m_handleRight->setVisible(!fullscreen);
        m_handleTop->setVisible(!fullscreen);
        m_handleBottom->setVisible(!fullscreen);
        m_handleTopLeft->setVisible(!fullscreen);
        m_handleTopRight->setVisible(!fullscreen);
        m_handleBottomLeft->setVisible(!fullscreen);
        m_handleBottomRight->setVisible(!fullscreen);

        WindowDTO dto;
        dto.x = x();
        dto.y = y();
        dto.width = m_windowedWidth;
        dto.height = m_windowedHeight;
        dto.isFullscreen = fullscreen;
        emit windowStateChanged(dto);
    }
    if (event->type() == QEvent::ActivationChange) {
        m_overlayWidget->setFocused(isActiveWindow());
    }
}

void Window::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D && event->modifiers() == Qt::ControlModifier) {
        emit debugRequested();
    }
    QWidget::keyPressEvent(event);
}

// When exiting fullscreen, the size gets reset to the previous size
// when it was still windowed, not fullscreen.
void Window::restoreWindowedSize() {
    resize(m_windowedWidth, m_windowedHeight);
}

void Window::restoreWindowedLocation() {
    move(m_windowedX, m_windowedY);
}




