#include "editor.h"
#include <QEvent>
#include <QPainter>
#include <QScrollArea>
#include "view_layer/theme.h"
#include <qscrollbar.h>
#include <shared/dto_model_to_view/editortextcontentsdto.h>

Editor::Editor(const EditorSettingsDTO &settings, QWidget *parent)
    : QWidget(parent)
{
    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);
    m_lineHeight = settings.lineHeight;
    m_fontScale = settings.fontScale;
}

void Editor::setSettings(const EditorSettingsDTO &settings)
{
    m_lineHeight = settings.lineHeight;
    m_fontScale = settings.fontScale;
    update();
}

void Editor::updateWidth(int width)
{
    if (m_contentWidth != width) {
        m_contentWidth = width;
        updateSize();
    }
}

void Editor::updateHeight(int height)
{
    if (m_contentHeight != height) {
        m_contentHeight = height;
        updateSize();
    }
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
        EditorVisibleLinesDTO dto{m_noOfVisibleLines, m_topLineIndex};
        emit editorStateChanged(dto);
    }
}

void Editor::updateLines(const EditorTextContentsDTO &dto)
{
    m_textLinesToDisplay = dto.textLinesToDisplay;
    updateHeight(dto.noOfAllLines * m_lineHeight);
    updateWidth((dto.noOfCharsOfLongestLine + 2) * m_fontAtlas.cellWidth() * m_fontScale);
    m_fileType = dto.fileType;
    update();
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
    }
    return QWidget::eventFilter(watched, event);
}

void Editor::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    for (int i = 0; i < m_textLinesToDisplay.size(); ++i) {
        float x = 5.f;
        float y = (m_topLineIndex + i) * m_lineHeight
                  + (m_lineHeight - m_fontAtlas.cellHeight()) / 2.f;
        m_fontRenderer
            ->drawText(painter, x, y, m_textLinesToDisplay[i], Theme::darkAmber(), m_fontScale);
    }
}
