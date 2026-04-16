#pragma once
#include <QWidget>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);

    void updateWidth(int width);
    void updateHeight(int height);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateSize();

    int m_contentWidth = 0;
    int m_contentHeight = 0;
};
