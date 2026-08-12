#pragma once
#include <QWidget>

class FontAtlas;
class FontRenderer;
class QScrollArea;
class CustomScrollBar;

class MainPopup : public QWidget
{
    Q_OBJECT
public:
    explicit MainPopup(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent = nullptr);
    void setHeaderText(const QString &text);
    QWidget *bodyWidget() const;

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    static constexpr int m_width = 500;
    static constexpr int m_height = 350;
    static constexpr int m_headerHeight = 40;

    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    QString m_headerText;

    QScrollArea *m_scrollArea;
    CustomScrollBar *m_verticalScrollBar;
    QWidget *m_bodyContent;
};
