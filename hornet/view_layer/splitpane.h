#pragma once

#include <QScrollArea>
#include <QSplitter>
#include <QSplitterHandle>
#include <QWidget>
#include <QScrollBar>

struct EditorCursorPosDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
class CustomScrollBar;
class Editor;

class SplitPaneHandle : public QSplitterHandle {
    Q_OBJECT

public:
    explicit SplitPaneHandle(int topPadding, Qt::Orientation orientation, QSplitter* parent);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int m_topPadding;
    bool m_hovered;
};

class SplitPane : public QSplitter {
    Q_OBJECT

public:
    explicit SplitPane(int leftWidth, int separatorTopPadding, QWidget* parent = nullptr);
    QWidget* leftPane() const;
    QWidget* rightPane() const;
    void updateEditorLines(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);

signals:
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorUserInputOccured(const EditorCursorPosDTO &dto);

protected:
    QSplitterHandle* createHandle() override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    int m_separatorTopPadding;
    QWidget* m_leftPane;
    QWidget* m_rightPane;
    QScrollArea* m_scrollArea;
    CustomScrollBar* m_verticalScrollBar;
    CustomScrollBar* m_horizontalScrollBar;
    QTimer* m_scrollBarHideTimer;
    Editor *m_editor;
};
