#pragma once
#include <QWidget>

class EditorContainer : public QWidget
{
    Q_OBJECT
public:
    explicit EditorContainer(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};
