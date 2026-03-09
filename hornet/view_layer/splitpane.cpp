#include "view_layer/splitpane.h"
#include "theme.h"
#include <QPainter>
#include <QPen>

SplitPaneHandle::SplitPaneHandle(int topPadding, Qt::Orientation orientation, QSplitter* parent)
    : QSplitterHandle(orientation, parent), m_topPadding(topPadding), m_hovered(false) {
    setCursor(Qt::SizeHorCursor);
}

void SplitPaneHandle::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), Theme::almostBlack());
    QPen pen(m_hovered ? Theme::darkGray() : Theme::darkerGray(), 1);
    painter.setPen(pen);
    painter.drawLine(width() - 1, m_topPadding, width() - 1, height());
}

void SplitPaneHandle::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
}

void SplitPaneHandle::leaveEvent(QEvent* event) {
    m_hovered = false;
    update();
}

SplitPane::SplitPane(int leftRatio, int rightRatio, int separatorTopPadding, QWidget* parent)
    : QSplitter(Qt::Horizontal, parent), m_separatorTopPadding(separatorTopPadding) {
    setHandleWidth(8);
    setChildrenCollapsible(false);

    m_leftPane  = new QWidget(this);
    m_rightPane = new QWidget(this);

    addWidget(m_leftPane);
    addWidget(m_rightPane);

    setStretchFactor(0, leftRatio);
    setStretchFactor(1, rightRatio);
}

QSplitterHandle* SplitPane::createHandle() {
    return new SplitPaneHandle(m_separatorTopPadding, orientation(), this);
}

QWidget* SplitPane::leftPane() const {
    return m_leftPane;
}

QWidget* SplitPane::rightPane() const {
    return m_rightPane;
}
