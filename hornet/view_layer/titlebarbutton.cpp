#include "view_layer/titlebarbutton.h"
#include "qdebug.h"
#include "theme.h"
#include <QPainter>
#include <QMouseEvent>

TitlebarButton::TitlebarButton(TitlebarButtonType type, QWidget* parent)
    : QWidget(parent), m_type(type), m_hovered(false), m_pressed(false), m_hoverColor(Theme::warmGray()), m_rightPadding(0) {
    setFixedSize(38, 38);
    setAttribute(Qt::WA_TranslucentBackground);
    m_pixmap = QPixmap(iconPath());
}

QString TitlebarButton::iconPath() const {
    switch (m_type) {
    case TitlebarButtonType::Close:    return ":/icons/close.png";
    case TitlebarButtonType::Minimize:
        return ":/icons/minimize.png";
    case TitlebarButtonType::Maximize: return ":/icons/maximize.png";
    case TitlebarButtonType::Tray:     return ":/icons/tray.png";
    case TitlebarButtonType::FileLoad:
        return ":/icons/load.png";
    case TitlebarButtonType::ProjectSave:
        return ":/icons/minimize.png";
    case TitlebarButtonType::ScriptRun:
        return ":/icons/maximize.png";
    }
    return "";
}

void TitlebarButton::setType(TitlebarButtonType type) {
    m_type = type;
    m_pixmap = QPixmap(iconPath());
    update();
}

void TitlebarButton::setHoverColor(const QColor& color) {
    m_hoverColor = color;
}

void TitlebarButton::setRightPadding(int padding) {
    m_rightPadding = padding;
    setFixedSize(width()+m_rightPadding, height());
}

void TitlebarButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // hover fill:
    if (m_hovered) {
        QColor hoverColor = m_pressed ? Theme::darkGray() : m_hoverColor;
        painter.fillRect(rect(), hoverColor);
    }

    // background shadow:
    if (!m_hovered) {
        int bgSize = 32;
        QPixmap bg(":/icons/titlebar_button_background.png");
        if (!bg.isNull()) {
            int bgX = ((width() - bgSize) / 2) - (m_rightPadding / 2);
            int bgY = (height() - bgSize) / 2;
            painter.drawPixmap(bgX, bgY, bgSize, bgSize, bg);
        }
    }

    // icon:
    int imageSize = 8;
    if (!m_pixmap.isNull()) {
        int x = ((width() - imageSize) / 2) - (m_rightPadding / 2);
        int y = ((height() - imageSize) / 2) + 1;
        if (m_hovered) {
            QPixmap recolored(m_pixmap.size());
            recolored.fill(Qt::transparent);
            QPainter p(&recolored);
            p.drawPixmap(0, 0, m_pixmap);
            p.setCompositionMode(QPainter::CompositionMode_SourceIn);
            p.fillRect(recolored.rect(), Qt::white);
            p.end();
            painter.drawPixmap(x, y, imageSize, imageSize, recolored);
        } else {
            painter.drawPixmap(x, y, imageSize, imageSize, m_pixmap);
        }
    }
}

void TitlebarButton::mouseMoveEvent(QMouseEvent* event) {
    bool inside = rect().contains(event->pos());
    if (m_hovered != inside) {
        m_hovered = inside;
        update();
    }
}

void TitlebarButton::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    update();
}

void TitlebarButton::leaveEvent(QEvent* event) {
    m_hovered = false;
    m_pressed = false;
    update();
}

void TitlebarButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
}

void TitlebarButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = false;
        if (m_hovered) {
            m_hovered = false;
            update();
            emit clicked();
        }
    }
}
