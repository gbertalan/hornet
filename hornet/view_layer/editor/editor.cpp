#include "editor.h"
#include <QEvent>
#include <QPainter>
#include <QScrollArea>
#include <qscrollbar.h>

Editor::Editor(const EditorSettingsDto &settings, QWidget *parent)
    : QWidget(parent)
{
    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);
    m_lineHeight = settings.lineHeight;
    m_fontScale = settings.fontScale;
}

void Editor::setSettings(const EditorSettingsDto &settings)
{
    m_lineHeight = settings.lineHeight;
    m_fontScale = settings.fontScale;
    update();
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

int Editor::calculateNoOfVisibleLines() const
{
    int containerHeight = parentWidget()->height();
    return (containerHeight / m_lineHeight) + 1;
}

int Editor::calculateTopLineIndex() const
{
    QScrollArea *scrollArea = qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
    if (!scrollArea || m_lineHeight == 0)
        return 0;

    int topLineIndex = scrollArea->verticalScrollBar()->value() / m_lineHeight;
    return topLineIndex;
}

void Editor::sendEditorState()
{
    int noOfVisLines = calculateNoOfVisibleLines();
    int topLineIndex = calculateTopLineIndex();
    if (noOfVisLines != m_noOfVisibleLines || topLineIndex != m_topLineIndex) {
        m_noOfVisibleLines = noOfVisLines;
        m_topLineIndex = topLineIndex;
        EditorVisibleLinesDto dto{m_noOfVisibleLines, m_topLineIndex};
        emit editorStateChanged(dto);
    }
}

void Editor::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (parentWidget()) {
        parentWidget()->installEventFilter(this);
        QScrollArea *scrollArea = qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
        if (scrollArea)
            connect(scrollArea->verticalScrollBar(),
                    &QScrollBar::valueChanged,
                    this,
                    &Editor::sendEditorState);
    }
    updateSize();
    sendEditorState();
}

bool Editor::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize) {
        updateSize();
        sendEditorState();
        ++count;
    }
    return QWidget::eventFilter(watched, event);
}

void Editor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::red);
    m_fontRenderer->drawText(painter, 20.f, 30.f, "Hello!", Qt::white, m_fontScale);
}
