#ifndef REGISTERDISPLAY_H
#define REGISTERDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QFont>
#include <QPaintEvent>
#include <QEnterEvent>
#include <QEvent>
#include <QPen>
#include <QColor>
#include <Qt>


class RegisterDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterDisplay(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    bool isHovered;

signals:
};

#endif // REGISTERDISPLAY_H
