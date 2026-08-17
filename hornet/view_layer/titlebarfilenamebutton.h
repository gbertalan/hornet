#pragma once
#include <QWidget>

class FontAtlas;
class FontRenderer;

class TitlebarFileNameButton : public QWidget
{
    Q_OBJECT

public:
    explicit TitlebarFileNameButton(FontAtlas &fontAtlas,
                                    FontRenderer &fontRenderer,
                                    QWidget *parent = nullptr);
    void setFileName(const QString &fileName);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QString m_fileName;
    bool m_hovered;
    bool m_pressed;
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
};
