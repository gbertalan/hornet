#include "view_layer/titlebarfilenamebutton.h"
#include <QMouseEvent>
#include <QPainter>
#include "theme.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"

TitlebarFileNameButton::TitlebarFileNameButton(FontAtlas &fontAtlas,
                                               FontRenderer &fontRenderer,
                                               QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
    , m_hovered(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::PointingHandCursor);
    setMinimumWidth(150);
}
void TitlebarFileNameButton::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    update();
}

void TitlebarFileNameButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_hovered)
        painter.fillRect(rect(), m_pressed ? Theme::darkGray() : Theme::warmGray());
    float scale = 0.5f;
    float y = (height() - m_fontAtlas.textHeight(scale)) / 2.f;
    m_fontRenderer.drawText(painter, 10.f, y, m_fileName, Theme::darkAmber(), scale);
}

void TitlebarFileNameButton::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    update();
}

void TitlebarFileNameButton::leaveEvent(QEvent *event)
{
    m_hovered = false;
    m_pressed = false;
    update();
}

void TitlebarFileNameButton::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        update();
    }
}

void TitlebarFileNameButton::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton) {
        m_pressed = false;
        if (m_hovered)
            emit clicked();
        update();
    }
}

void TitlebarFileNameButton::mouseMoveEvent(QMouseEvent *event)
{
    bool inside = rect().contains(event->pos());
    if (m_hovered != inside) {
        m_hovered = inside;
        update();
    }
    event->accept();
}
