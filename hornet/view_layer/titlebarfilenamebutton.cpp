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
    else
        painter.fillRect(rect(), Theme::almostBlack());

    painter.setPen(Theme::darkerGray());
    painter.drawLine(0, 0, 0, height() - 2);
    painter.drawLine(width() - 1, 0, width() - 1, height() - 2);

    const float scale = 0.7f;
    const float textPadding = 10.f;
    const float availableWidth = static_cast<float>(width()) - (textPadding * 2.0f);

    QString displayText = m_fileName;
    float displayTextWidth = m_fontAtlas.textWidth(displayText.length(), scale);

    float textX;
    if (displayTextWidth > availableWidth) {
        const float ellipsisWidth = m_fontAtlas.textWidth(3, scale);
        const float maxTextWidth = availableWidth - ellipsisWidth;
        while (displayText.length() > 0) {
            displayTextWidth = m_fontAtlas.textWidth(displayText.length(), scale);
            if (displayTextWidth <= maxTextWidth)
                break;
            displayText = displayText.left(displayText.length() - 1);
        }
        displayText += "...";
        textX = textPadding;
    } else {
        textX = (static_cast<float>(width()) - displayTextWidth) / 2.0f;
    }

    const float textVisualHeight = static_cast<float>(m_fontAtlas.getAscenderPx()
                                                      + m_fontAtlas.getDescenderPx())
                                   * scale;
    const float y = (static_cast<float>(height()) - textVisualHeight) / 2.0f;
    m_fontRenderer.drawText(painter, textX, y, displayText, Theme::darkAmber(), scale);
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
