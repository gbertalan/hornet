#include "view_layer/resizehandle.h"
#include <QMouseEvent>

ResizeHandle::ResizeHandle(ResizeEdge edge, QWidget* parent)
    : QWidget(parent), m_edge(edge), m_resizing(false) {

    switch (m_edge) {
    case ResizeEdge::Left:
    case ResizeEdge::Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case ResizeEdge::Top:
    case ResizeEdge::Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case ResizeEdge::TopLeft:
    case ResizeEdge::BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case ResizeEdge::TopRight:
    case ResizeEdge::BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    }
}

void ResizeHandle::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_resizing = true;
        m_startPos = event->globalPos();
        m_startGeometry = window()->geometry();
    }
}

void ResizeHandle::mouseMoveEvent(QMouseEvent* event) {
    if (!m_resizing) return;

    QPoint delta = event->globalPos() - m_startPos;
    QRect geo = m_startGeometry;

    switch (m_edge) {
    case ResizeEdge::Left:
        geo.setLeft(geo.left() + delta.x());
        break;
    case ResizeEdge::Right:
        geo.setRight(geo.right() + delta.x());
        break;
    case ResizeEdge::Top:
        geo.setTop(geo.top() + delta.y());
        break;
    case ResizeEdge::Bottom:
        geo.setBottom(geo.bottom() + delta.y());
        break;
    case ResizeEdge::TopLeft:
        geo.setTopLeft(geo.topLeft() + delta);
        break;
    case ResizeEdge::TopRight:
        geo.setTopRight(geo.topRight() + delta);
        break;
    case ResizeEdge::BottomLeft:
        geo.setBottomLeft(geo.bottomLeft() + delta);
        break;
    case ResizeEdge::BottomRight:
        geo.setBottomRight(geo.bottomRight() + delta);
        break;
    }

    if (geo.width() < window()->minimumWidth()) {
        geo.setWidth(window()->minimumWidth());
        if (m_edge == ResizeEdge::Left || m_edge == ResizeEdge::TopLeft || m_edge == ResizeEdge::BottomLeft) {
            geo.moveRight(m_startGeometry.right());
        }
    }

    if (geo.height() < window()->minimumHeight()) {
        geo.setHeight(window()->minimumHeight());
        if (m_edge == ResizeEdge::Top || m_edge == ResizeEdge::TopLeft || m_edge == ResizeEdge::TopRight) {
            geo.moveBottom(m_startGeometry.bottom());
        }
    }

    window()->setGeometry(geo);
}

void ResizeHandle::mouseReleaseEvent(QMouseEvent* event) {
    m_resizing = false;
}
