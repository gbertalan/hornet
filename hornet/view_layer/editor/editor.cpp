#include "editor.h"
#include <QEvent>
#include <QPainter>

Editor::Editor(QWidget *parent)
    : QWidget(parent)
{
    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);
}

void Editor::updateWidth(int width)
{
    m_contentWidth = width;
    updateSize();
}

void Editor::updateHeight(int height)
{
    m_contentHeight = height;
    updateSize();
}

void Editor::updateSize()
{
    const QWidget *viewport = parentWidget();
    int w = viewport ? std::max(m_contentWidth, viewport->width()) : m_contentWidth;
    int h = viewport ? std::max(m_contentHeight, viewport->height()) : m_contentHeight;
    resize(w, h);
}

void Editor::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (parentWidget())
        parentWidget()->installEventFilter(this);
    updateSize();
}

bool Editor::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize)
        updateSize();
    return QWidget::eventFilter(watched, event);
}

void Editor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::red);
    m_fontRenderer->drawText(painter, 20.f, 30.f, "Hello!", Qt::white, 1.0f);
}
