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
    setSettings(settings);
    connect(&m_cursorTimer, &QTimer::timeout, this, [this]() {
        m_cursorVisible = !m_cursorVisible;
        update(cursorRect(m_cursorX, m_cursorY));
        emit cursorBlinkToggled(m_cursorVisible);
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
    m_isTerminal = settings.isTerminal;
    if (!m_isTerminal)
        m_textUniColor = Theme::darkAmber();
    else
        m_textUniColor = Theme::desaturatedTeal();
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
    m_terminalPrompts = dto.terminalPrompts;
    updateHeight(m_noOfAllLines * m_lineHeight + (m_lineHeight / 2.f));
    updateWidth(5.f + leftColumnWidth()
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
    float leftMargin = 5.f;
    float textX = leftMargin + leftColumnWidth();
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;
    int digits = QString::number(m_noOfAllLines).length();
    for (int i = 0; i < m_textLinesToDisplay.size(); ++i) {
        float lineTop = (m_topLineIndex + i) * m_lineHeight;
        float y = lineTop + verticalPadding - topMargin;
        // drawLineDebugBackground(painter, i, y);
        drawLineNumber(painter, i, digits, leftMargin, y);
        drawTerminalPrompt(painter, i, leftMargin + lineNumberSectionWidth(), y);
        drawLineText(painter, i, textX, y);
        drawCursor(painter, i, textX, y, verticalPadding);
    }
}

void Editor::drawLineDebugBackground(QPainter &painter, int index, float y)
{
    static const QColor colors[] = {
        QColor(255, 0, 0, 40),
        QColor(0, 255, 0, 40),
        QColor(0, 0, 255, 40),
        QColor(255, 255, 0, 40),
    };
    QColor color = colors[index % 4];

    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;

    painter.fillRect(QRectF(0, topMargin + y + 1, width(), m_lineHeight), color);
}

void Editor::drawLineNumber(QPainter &painter, int index, int digits, float leftMargin, float y)
{
    float lineNumberWidth = m_fontAtlas.textWidth(QString::number(m_noOfAllLines).length() + 2,
                                                  m_fontScale);
    QString lineNumber = QString::number(m_topLineIndex + index + 1);
    float numberWidth = m_fontAtlas.textWidth(lineNumber.length(), m_fontScale);
    float numberX = (lineNumberWidth - numberWidth) / 2.f;
    if (index == m_cursorY)
        m_fontRenderer->drawText(painter, numberX, y, lineNumber, Theme::brightYellow(), m_fontScale);
    else
        m_fontRenderer->drawText(painter, numberX, y, lineNumber, Theme::darkGray(), m_fontScale);
    // gray separator line:
    painter.save();
    painter.setPen(QPen(Theme::darkGray(), 1));
    painter.drawLine(QPointF(lineNumberWidth, y), QPointF(lineNumberWidth, y + m_lineHeight));
    painter.restore();
}

void Editor::drawLineText(QPainter &painter, int index, float textX, float y)
{
    if (m_isTerminal && index != m_cursorY)
        m_fontRenderer->drawText(painter,
                                 textX,
                                 y,
                                 m_textLinesToDisplay[index],
                                 Theme::darkGray(),
                                 m_fontScale);
    else
        m_fontRenderer
            ->drawText(painter, textX, y, m_textLinesToDisplay[index], m_textUniColor, m_fontScale);
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
    float textX = 5.f + leftColumnWidth();
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
    float cursorPixelX = 5.f + leftColumnWidth() + m_fontAtlas.textWidth(m_cursorX, m_fontScale);
    float lineTop = m_cursorY * m_lineHeight;
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float topMargin = verticalPadding / 2.f;
    float y = lineTop + verticalPadding - topMargin;
    float charWidth = m_fontAtlas.textWidth(1, m_fontScale);
    return QRectF(cursorPixelX, y + verticalPadding, charWidth, m_lineHeight).toRect();
}

void Editor::updateCursorPosition(const EditorCursorPosDTO &dto)
{
    float fontHeight = m_fontAtlas.textHeight(m_fontScale);
    float verticalPadding = (m_lineHeight - fontHeight) / 2.f;
    float lineTop = m_cursorY * m_lineHeight + verticalPadding + 1;
    update(QRectF(0, lineTop, width(), m_lineHeight).toRect());
    m_cursorX = dto.cursorX;
    m_cursorY = dto.cursorY;
    m_cursorVisible = true;
    m_cursorTimer.start(200);
    emit cursorBlinkToggled(m_cursorVisible);

    float newLineTop = m_cursorY * m_lineHeight + verticalPadding + 1;
    update(QRectF(0, newLineTop, width(), m_lineHeight).toRect());
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

    if (ctrl && event->key() == Qt::Key_D) {
        emit editorKeyPressed(
            EditorKeyPressDTO(0, EditorKeyPressDTO::SpecialKey::CtrlD, ctrl, shift, alt));
        return;
    }

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
    case Qt::Key_Return:
    case Qt::Key_Enter:
        specialKey = EditorKeyPressDTO::SpecialKey::Enter;
        break;
    case Qt::Key_Home:
        specialKey = EditorKeyPressDTO::SpecialKey::Home;
        break;
    case Qt::Key_End:
        specialKey = EditorKeyPressDTO::SpecialKey::End;
        break;
    case Qt::Key_PageUp:
        specialKey = EditorKeyPressDTO::SpecialKey::PageUp;
        break;
    case Qt::Key_PageDown:
        specialKey = EditorKeyPressDTO::SpecialKey::PageDown;
        break;
    case Qt::Key_Tab:
        specialKey = EditorKeyPressDTO::SpecialKey::Tab;
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
    float textX = 5.f + leftColumnWidth();
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

float Editor::lineNumberSectionWidth() const
{
    int digits = QString::number(m_noOfAllLines).length();
    return m_fontAtlas.textWidth(digits + 2, m_fontScale);
}

float Editor::leftColumnWidth() const
{
    float lnWidth = lineNumberSectionWidth();
    if (!m_isTerminal || m_terminalPrompts.isEmpty())
        return lnWidth;
    int maxPromptLen = 0;
    for (const QString &p : m_terminalPrompts)
        maxPromptLen = std::max(maxPromptLen, static_cast<int>(p.length()));
    return lnWidth + m_fontAtlas.textWidth(maxPromptLen + 2, m_fontScale);
}

void Editor::drawTerminalPrompt(QPainter &painter, int index, float x, float y)
{
    int lineIndex = m_topLineIndex + index;
    if (lineIndex >= m_terminalPrompts.size())
        return;
    const QString &prompt = m_terminalPrompts[lineIndex];
    if (prompt.isEmpty())
        return;
    if (index == m_cursorY)
        m_fontRenderer->drawText(painter, x, y, prompt, Theme::darkAmber(), m_fontScale);
    else
        m_fontRenderer->drawText(painter, x, y, prompt, Theme::darkGray(), m_fontScale);
}
