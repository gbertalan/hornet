#pragma once
#include <QWidget>

class TitlebarFileNameButton : public QWidget
{
    Q_OBJECT

public:
    explicit TitlebarFileNameButton(QWidget *parent = nullptr);
    void setFileName(const QString &fileName);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString m_fileName;
    bool m_hovered;
    bool m_pressed;
};
