#pragma once

#include <QSplitter>
#include <QSplitterHandle>
#include <QWidget>

class SplitPaneHandle : public QSplitterHandle {
    Q_OBJECT

public:
    explicit SplitPaneHandle(int topPadding, Qt::Orientation orientation, QSplitter* parent);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_topPadding;
};

class SplitPane : public QSplitter {
    Q_OBJECT

public:
    explicit SplitPane(int leftRatio, int rightRatio, int separatorTopPadding, QWidget* parent = nullptr);

    QWidget* leftPane() const;
    QWidget* rightPane() const;

protected:
    QSplitterHandle* createHandle() override;

private:
    int m_separatorTopPadding;
    QWidget* m_leftPane;
    QWidget* m_rightPane;
};
