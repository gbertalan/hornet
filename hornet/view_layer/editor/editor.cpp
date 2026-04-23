#include "editor.h"
#include <QEvent>
#include <QPainter>
#include <QScrollArea>
#include "shared/dto_bidirectional/editorsettingsdto.h"
#include "shared/dto_view_to_model/editorcursorposdto.h"
#include "shared/dto_view_to_model/editorkeypressdto.h"
#include "shared/dto_view_to_model/editorvisiblelinesdto.h"
#include "view_layer/theme.h"
#include <qevent.h>
#include <qscrollbar.h>
#include <shared/dto_model_to_view/editorviewstatedto.h>

Editor::Editor(const EditorSettingsDTO &settings, QWidget *parent)
    : QWidget(parent)
{
    m_fontAtlas.addFont(":/fonts/JetBrainsMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansMono-Bold.ttf");
    m_fontAtlas.addFont(":/fonts/NotoSansCJK-Regular.ttc");
    m_fontRenderer = std::make_unique<FontRenderer>(m_fontAtlas);

    m_lineHeight = settings.lineHeight;
    m_fontScale = settings.fontScale;

    connect(&m_cursorTimer, &QTimer::timeout, this, [this]() {
        m_cursorVisible = !m_cursorVisible;
        update(cursorRect(m_cursorX, m_cursorY));
    });
    m_cursorTimer.start(200);

    setFocusPolicy(Qt::StrongFocus);
    QTimer::singleShot(0, this, [this]() { setFocus(); }); // gets focus, after constructor is ready.

    setMouseTracking(true);
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

void Editor::updateEditorState(const EditorViewStateDTO &dto)
{
    m_textLinesToDisplay = dto.textLinesToDisplay;
    m_noOfAllLines = dto.noOfAllLines;
    m_noOfCharsOfLongestLine = dto.noOfCharsOfLongestLine;
    m_fileType = dto.fileType;

    updateHeight(m_noOfAllLines * m_lineHeight + (m_lineHeight / 2.f));

    int digits = QString::number(m_noOfAllLines).length();
    float lineNumberSectionWidth = 5.f + m_fontAtlas.textWidth(digits + 2, m_fontScale);
    updateWidth(lineNumberSectionWidth
                + m_fontAtlas.textWidth(m_noOfCharsOfLongestLine + 2, m_fontScale));

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
    int digits = QString::number(m_noOfAllLines).length();
    float lineNumberWidth = m_fontAtlas.textWidth(digits + 2, m_fontScale);
    float leftMargin = 5.f;
    float textX = leftMargin + lineNumberWidth;
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;
    for (int i = 0; i < m_textLinesToDisplay.size(); ++i) {
        float lineTop = (m_topLineIndex + i) * m_lineHeight;
        float y = lineTop + verticalPadding - topMargin;
        drawLineNumber(painter, i, digits, leftMargin, y);
        drawLineText(painter, i, textX, y);
        drawCursor(painter, i, textX, y, verticalPadding);
    }
}

void Editor::drawLineNumber(QPainter &painter, int index, int digits, float leftMargin, float y)
{
    QString lineNumber = QString::number(m_topLineIndex + index + 1);
    float numberX = leftMargin + m_fontAtlas.textWidth(digits - lineNumber.length(), m_fontScale);
    m_fontRenderer->drawText(painter, numberX, y, lineNumber, Theme::darkGray(), m_fontScale);
}

void Editor::drawLineText(QPainter &painter, int index, float textX, float y)
{
    m_fontRenderer
        ->drawText(painter, textX, y, m_textLinesToDisplay[index], Theme::darkAmber(), m_fontScale);
}

void Editor::drawCursor(QPainter &painter, int index, float textX, float y, float verticalPadding)
{
    if (!m_cursorVisible)
        return;
    if (m_topLineIndex + index != m_cursorY)
        return;
    float charWidth = m_fontAtlas.textWidth(1, m_fontScale);
    float cursorPixelX = textX + m_fontAtlas.textWidth(m_cursorX, m_fontScale);
    painter.fillRect(QRectF(cursorPixelX, y + 2, charWidth, m_lineHeight), Theme::brightAmber());
    if (m_cursorX < m_textLinesToDisplay[index].length()) {
        QString cursorChar = m_textLinesToDisplay[index][m_cursorX];
        m_fontRenderer
            ->drawText(painter, cursorPixelX, y, cursorChar, Theme::almostBlack(), m_fontScale);
    }
}

void Editor::mouseReleaseEvent(QMouseEvent *event)
{
    int digits = QString::number(m_noOfAllLines).length();
    float lineNumberWidth = m_fontAtlas.textWidth(digits + 2, m_fontScale);
    float textX = 5.f + lineNumberWidth;

    float charWidth = m_fontAtlas.textWidth(1, m_fontScale);
    int cursorX = static_cast<int>((event->pos().x() - textX) / charWidth);
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;
    int cursorY = static_cast<int>((event->pos().y() - verticalPadding - topMargin)
                                   / (m_lineHeight));

    cursorX = std::max(0, cursorX);
    cursorY = std::max(0, cursorY);

    EditorCursorPosDTO dto{cursorX, cursorY};
    emit editorCursorPosChanged(dto);
}

QRect Editor::cursorRect(int cursorX, int cursorY) const
{
    float lineNumberWidth = m_fontAtlas.textWidth(QString::number(m_noOfAllLines).length() + 2,
                                                  m_fontScale);
    float cursorPixelX = 5.f + lineNumberWidth + m_fontAtlas.textWidth(m_cursorX, m_fontScale);
    float lineTop = (m_cursorY) *m_lineHeight;
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;
    float y = lineTop + verticalPadding - topMargin;
    float charWidth = m_fontAtlas.textWidth(1, m_fontScale);
    return QRectF(cursorPixelX, y + verticalPadding, charWidth, m_lineHeight).toRect();
}

void Editor::updateCursorPosition(const EditorCursorPosDTO &dto)
{
    update(cursorRect(m_cursorX, m_cursorY));
    m_cursorX = dto.cursorX;
    m_cursorY = dto.cursorY;
    m_cursorVisible = true;
    m_cursorTimer.start(200);
    update(cursorRect(m_cursorX, m_cursorY));
    scrollToCursor();
}

void Editor::scrollToCursor()
{
    QScrollArea *scrollArea = qobject_cast<QScrollArea *>(parentWidget()->parentWidget());
    if (!scrollArea)
        return;
    QRect rect = cursorRect(m_cursorX, m_cursorY);

    int noOfCharsAsMargin = 5;
    float horizontalScrollMargin = m_fontAtlas.textWidth(noOfCharsAsMargin, m_fontScale);
    int noOfRowsAsMargin = 3;
    float verticalScrollMargin = m_lineHeight * noOfRowsAsMargin;
    scrollArea->ensureVisible(rect.x(), rect.y(), horizontalScrollMargin, verticalScrollMargin);
}

void Editor::keyPressEvent(QKeyEvent *event)
{
    bool ctrl = event->modifiers() & Qt::ControlModifier;
    bool shift = event->modifiers() & Qt::ShiftModifier;
    bool alt = event->modifiers() & Qt::AltModifier;

    EditorKeyPressDTO::SpecialKey specialKey = EditorKeyPressDTO::SpecialKey::None;
    switch (event->key()) {
    case Qt::Key_Left:
        specialKey = EditorKeyPressDTO::SpecialKey::Left;
        break;
    case Qt::Key_Right:
        specialKey = EditorKeyPressDTO::SpecialKey::Right;
        break;
    case Qt::Key_Up:
        specialKey = EditorKeyPressDTO::SpecialKey::Up;
        break;
    case Qt::Key_Down:
        specialKey = EditorKeyPressDTO::SpecialKey::Down;
        break;
    case Qt::Key_Backspace:
        specialKey = EditorKeyPressDTO::SpecialKey::Backspace;
        break;
    case Qt::Key_Delete:
        specialKey = EditorKeyPressDTO::SpecialKey::Delete;
        break;
    }

    if (specialKey != EditorKeyPressDTO::SpecialKey::None) {
        emit editorKeyPressed(EditorKeyPressDTO(0, specialKey, ctrl, shift, alt));
        return;
    }

    QString text = event->text();
    if (text.isEmpty())
        return;

    char32_t key = text.toUcs4().first();
    emit editorKeyPressed(EditorKeyPressDTO(key, specialKey, ctrl, shift, alt));
}

void Editor::mouseMoveEvent(QMouseEvent *event)
{
    float lineNumberWidth = m_fontAtlas.textWidth(QString::number(m_noOfAllLines).length() + 2,
                                                  m_fontScale);
    float textX = 5.f + lineNumberWidth;

    bool overText = event->pos().x() >= textX;
    if (overText == m_isTextCursor)
        return;

    m_isTextCursor = overText;
    setCursor(overText ? Qt::IBeamCursor : Qt::ArrowCursor);
}

void Editor::focusOutEvent(QFocusEvent *event)
{
    // Reclaim focus if lost to scrollbar clicks so keyboard input keeps working
    if (event->reason() == Qt::MouseFocusReason)
        setFocus();
}
