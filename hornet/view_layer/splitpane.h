#pragma once

#include <QScrollArea>
#include <QSplitter>
#include <QSplitterHandle>
#include <QWidget>
#include <QScrollBar>

class CustomScrollBar;

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

protected:
    QSplitterHandle* createHandle() override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    int m_separatorTopPadding;
    QWidget* m_leftPane;
    QWidget* m_rightPane;
    QScrollArea* m_scrollArea;
    QWidget* m_editorWidget;
    CustomScrollBar* m_verticalScrollBar;
    CustomScrollBar* m_horizontalScrollBar;
    QTimer* m_scrollBarHideTimer;

public:
    QWidget* editorWidget() const;
};
