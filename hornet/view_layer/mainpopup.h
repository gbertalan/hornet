#pragma once
#include <QWidget>

class MainPopup : public QWidget
{
    Q_OBJECT
public:
    explicit MainPopup(QWidget *parent = nullptr);

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    static constexpr int m_width = 500;
    static constexpr int m_height = 350;
};
