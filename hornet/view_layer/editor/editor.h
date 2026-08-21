#pragma once
#include <QTimer>
#include <QWidget>
#include <memory>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

class MarkRange;
struct EditorKeyPressDTO;
class FontRenderer;
struct EditorVisibleLinesDTO;
struct EditorSettingsDTO;
struct EditorViewStateDTO;
struct EditorCursorPosDTO;
struct EditorSelectionDTO;
struct EditorMarksDTO;

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(const EditorSettingsDTO &settings,
                    FontAtlas &fontAtlas,
                    FontRenderer &fontRenderer,
                    QWidget *parent = nullptr);

    // ================================================================
    // SLICE: settings + sizing (public API called by SplitPane/Control)
    // ================================================================
    void setSettings(const EditorSettingsDTO &settings);
    void updateWidth(int width);
    void updateHeight(int height);

    // ================================================================
    // SLICE: Model -> View state push
    // ================================================================
    void updateEditorState(const EditorViewStateDTO &dto);
    void updateCursorPosition(const EditorCursorPosDTO &dto);
    void updateEditorSelection(const EditorSelectionDTO &dto);
    void updateEditorMarks(const EditorMarksDTO &dto);

signals:
    // ================================================================
    // SLICE: View -> Control (Editor's own state/input reported upward)
    // ================================================================
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);
    void editorSelectionChanged(const EditorSelectionDTO &dto);

    // ================================================================
    // SLICE: direct sibling wiring (Editor -> Grid, bypasses Control)
    // ================================================================
    void cursorBlinkToggled(bool visible);
    void ctrlStateChanged(bool isCtrlPressed);

protected:
    // ================================================================
    // SLICE: rendering
    // ================================================================
    void paintEvent(QPaintEvent *event) override;

    // ================================================================
    // SLICE: Qt lifecycle / sizing plumbing
    // ================================================================
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    // ================================================================
    // SLICE: input handling (mouse + keyboard)
    // ================================================================
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    bool focusNextPrevChild(bool next) override
    {
        return false;
    } // prevent taking away focus when tab is pressed
private:
    // ================================================================
    // SLICE: sizing / scroll-state-reporting helpers
    // ================================================================
    void updateSize();
    int calculateNoOfVisibleLines() const;
    int calculateTopLineIndex() const;
    void sendEditorState();
    void scrollToCursor();

    // ================================================================
    // SLICE: rendering helpers (called from paintEvent)
    // ================================================================
    void drawLineDebugBackground(QPainter &painter, int index, float y);
    void drawLineNumber(QPainter &painter, int index, int digits, float leftMargin, float y);
    void drawTerminalPrompt(QPainter &painter, int index, float x, float y);
    void drawLineText(QPainter &painter, int index, float textX, float y);
    void drawCursor(QPainter &painter, int index, float textX, float y, float verticalPadding);
    void drawSelection(QPainter &painter, int index, float textX, float lineTop);
    void drawMarks(QPainter &painter, int index, float lineTop);

    // ================================================================
    // SLICE: geometry-measurement helpers
    // ================================================================
    QRect cursorRect() const;
    float leftColumnWidth() const;
    float lineNumberSectionWidth() const;
    std::pair<int, int> getCursorPosFromMouse(QPoint mousePos);

    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;

    int m_lineHeight;
    float m_fontScale;
    int m_contentWidth = 0;
    int m_contentHeight = 0;
    int m_noOfVisibleLines = 0;
    int m_topLineIndex = 0;
    int m_noOfAllLines = 0;
    int m_noOfCharsOfLongestLine = 0;
    QString m_fileType;
    QVector<QString> m_textLinesToDisplay;
    int m_cursorX = 0; // column
    int m_cursorY = 0; // row
    QTimer m_cursorTimer;
    bool m_cursorVisible = true;
    bool m_isTextCursor = false;
    bool m_isTerminal = false;
    QColor m_textUniColor;
    QVector<QString> m_terminalPrompts;

    bool m_isSelecting = false;
    bool m_hasSelection = false;
    int m_selectionAnchorX = 0;
    int m_selectionAnchorY = 0;
    int m_selectionExtentX = 0;
    int m_selectionExtentY = 0;

    QVector<MarkRange> m_marks;
};