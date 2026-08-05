#include "view_layer/window.h"
#include <QBitmap>
#include <QEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QPalette>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "shared/dto_view_to_model/windowdto.h"
#include "theme.h"
#include "view_layer/overlaywidget.h"
#include "view_layer/resizehandle.h"
#include "view_layer/splitpane.h"
#include "view_layer/titlebar.h"

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

    m_splitPane = new SplitPane(800, 0, this);
    m_splitPane->setGeometry(0, 0, width(), height());

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

    connect(m_titleBar, &TitleBar::windowCloseClicked,    this, &Window::windowCloseClicked);
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

    connect(m_splitPane, &SplitPane::editorStateChanged, this, &Window::editorStateChanged);
    connect(m_splitPane, &SplitPane::editorCursorPosChanged, this, &Window::editorCursorPosChanged);
    connect(m_splitPane, &SplitPane::editorKeyPressed, this, &Window::editorKeyPressed);
    connect(m_splitPane, &SplitPane::gridZoomChanged, this, &Window::gridZoomChanged);
    connect(m_splitPane, &SplitPane::gridDragged, this, &Window::gridDragged);
    connect(m_splitPane, &SplitPane::boxDragged, this, &Window::boxDragged);
    connect(m_splitPane, &SplitPane::boxSelected, this, &Window::boxSelected);
    connect(m_splitPane, &SplitPane::boxResized, this, &Window::boxResized);
    connect(m_splitPane, &SplitPane::boxUnloadRequested, this, &Window::boxUnloadRequested);
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

void Window::updateEditorState(const EditorViewStateDTO &dto)
{
    m_splitPane->updateEditorState(dto);
}

void Window::updateEditorCursorPos(const EditorCursorPosDTO &dto)
{
    m_splitPane->updateEditorCursorPos(dto);
}

void Window::updateEditorSettings(const EditorSettingsDTO &dto)
{
    m_splitPane->updateEditorSettings(dto);
}

void Window::updateGridViewState(const GridViewStateDTO &dto)
{
    m_splitPane->updateGridViewState(dto);
}

void Window::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_splitPane->setGeometry(0, 0, width(), height());
    m_overlayWidget->setGeometry(0, 0, width(), height());
    positionResizeHandles();
    if(!isFullScreen()){
        m_windowedWidth = width();
        m_windowedHeight = height();

        int x = this->x();
        int y = this->y();
        int width = this->width();
        int height = this->height();
        bool isFullscreen = false;
        WindowDTO dto{x, y, width, height, isFullscreen};
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

        int x = this->x();
        int y = this->y();
        int width = this->width();
        int height = this->height();
        bool isFullscreen = false;
        WindowDTO dto{x, y, width, height, isFullscreen};
        emit windowStateChanged(dto);
    }
}

void Window::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        bool isFullscreen = isFullScreen();
        m_overlayWidget->setFullscreen(isFullscreen);
        m_titleBar->setFullscreen(isFullscreen);

        m_handleLeft->setVisible(!isFullscreen);
        m_handleRight->setVisible(!isFullscreen);
        m_handleTop->setVisible(!isFullscreen);
        m_handleBottom->setVisible(!isFullscreen);
        m_handleTopLeft->setVisible(!isFullscreen);
        m_handleTopRight->setVisible(!isFullscreen);
        m_handleBottomLeft->setVisible(!isFullscreen);
        m_handleBottomRight->setVisible(!isFullscreen);

        int x = this->x();
        int y = this->y();
        int width = m_windowedWidth;
        int height = m_windowedHeight;
        WindowDTO dto{x, y, width, height, isFullscreen};

        emit windowStateChanged(dto);
    }
    if (event->type() == QEvent::ActivationChange) {
        m_overlayWidget->setFocused(isActiveWindow());
    }
}

// When exiting fullscreen, the size gets reset to the previous size
// when it was still windowed, not fullscreen.
void Window::restoreWindowedSize() {
    resize(m_windowedWidth, m_windowedHeight);
}

void Window::restoreWindowedLocation() {
    move(m_windowedX, m_windowedY);
}




