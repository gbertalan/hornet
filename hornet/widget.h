#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QPushButton;
class RegisterDisplay;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPushButton *button;
    RegisterDisplay *regDisplay;
};

#endif // WIDGET_H
