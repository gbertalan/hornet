#pragma once

#include <QScrollArea>
#include <QSplitter>
#include <QSplitterHandle>
#include <QWidget>
#include <QScrollBar>

#include <view_layer/grid/grid.h>

struct EditorKeyPressDTO;
struct EditorCursorPosDTO;
struct EditorVisibleLinesDTO;
struct EditorViewStateDTO;
struct EditorSettingsDTO;
struct BoxResizeDTO;
struct BoxUnloadRequestedDTO;
struct ToolButtonActivatedDTO;
class CustomScrollBar;
class Editor;
class FontAtlas;
class FontRenderer;

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
    explicit SplitPane(int leftWidth,
                       int separatorTopPadding,
                       FontAtlas &fontAtlas,
                       FontRenderer &fontRenderer,
                       QWidget *parent = nullptr);
    QWidget* leftPane() const;
    QWidget* rightPane() const;
    void updateEditorState(const EditorViewStateDTO &dto);
    void updateEditorCursorPos(const EditorCursorPosDTO &dto);
    void updateEditorSettings(const EditorSettingsDTO &dto);
    void updateGridViewState(const GridViewStateDTO &dto);
    int leftPaneWidth() const;
    void focusEditor();

signals:
    void editorStateChanged(const EditorVisibleLinesDTO &dto);
    void editorCursorPosChanged(const EditorCursorPosDTO &dto);
    void editorKeyPressed(const EditorKeyPressDTO &dto);
    void gridZoomChanged(const GridZoomDTO &dto);
    void gridDragged(const GridDragDTO &dto);
    void boxDragged(const BoxDragDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);
    void boxResized(const BoxResizeDTO &dto);
    void boxUnloadRequested(const BoxUnloadRequestedDTO &dto);
    void toolButtonActivated(const ToolButtonActivatedDTO &dto);
    void leftPaneWidthChanged(int width);

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
    Grid *m_grid;
};
