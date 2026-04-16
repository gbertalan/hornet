#pragma once
#include <QWidget>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

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

    FontAtlas m_fontAtlas;
    std::unique_ptr<FontRenderer> m_fontRenderer;

    int m_contentWidth = 0;
    int m_contentHeight = 0;
};
