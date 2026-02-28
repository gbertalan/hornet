#include "view_layer/view.h"
#include "view_layer/titlebar.h"
#include "view_layer/resizehandle.h"
#include "shared/dto/numberdto.h"
#include "theme.h"
#include <QMessageBox>
#include <QPalette>
#include <QVBoxLayout>

static constexpr int HANDLE_THICKNESS = 6;
static constexpr int CORNER_SIZE = 12;

View::View(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(300, 200);
    resize(800, 600);

    QPalette palette;
    palette.setColor(QPalette::Window, Theme::almostBlack());
    setPalette(palette);
    setAutoFillBackground(true);

    m_titleBar = new TitleBar(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_titleBar);
    layout->addStretch();
    setLayout(layout);

    setupResizeHandles();
    positionResizeHandles();
}

void View::setupResizeHandles() {
    m_handleLeft        = new ResizeHandle(ResizeEdge::Left, this);
    m_handleRight       = new ResizeHandle(ResizeEdge::Right, this);
    m_handleTop         = new ResizeHandle(ResizeEdge::Top, this);
    m_handleBottom      = new ResizeHandle(ResizeEdge::Bottom, this);
    m_handleTopLeft     = new ResizeHandle(ResizeEdge::TopLeft, this);
    m_handleTopRight    = new ResizeHandle(ResizeEdge::TopRight, this);
    m_handleBottomLeft  = new ResizeHandle(ResizeEdge::BottomLeft, this);
    m_handleBottomRight = new ResizeHandle(ResizeEdge::BottomRight, this);
}

void View::positionResizeHandles() {
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

void View::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    positionResizeHandles();
}

void View::displayNumber(const NumberDTO& dto) {
}

void View::showError(const QString& message) {
    QMessageBox::warning(this, "Error", message, QMessageBox::Ok);
}
