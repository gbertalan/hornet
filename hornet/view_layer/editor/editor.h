#pragma once
#include <QWidget>
#include <memory>
#include <shared/dto_bidirectional/editorsettingsdto.h>
#include <shared/dto_view_to_model/editorvisiblelinesdto.h>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(const EditorSettingsDTO &settings, QWidget *parent = nullptr);

    void updateWidth(int width);
    void updateHeight(int height);
    void setSettings(const EditorSettingsDTO &settings);

signals:
    void editorStateChanged(const EditorVisibleLinesDTO &dto);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void updateSize();
    int calculateNoOfVisibleLines() const;
    int calculateTopLineIndex() const;
    void sendEditorState();

    FontAtlas m_fontAtlas;
    std::unique_ptr<FontRenderer> m_fontRenderer;

    int m_lineHeight = 0;
    float m_fontScale = 0.0f;

    int m_contentWidth = 0;
    int m_contentHeight = 0;

    int m_noOfVisibleLines = 0;
    int m_topLineIndex = 0;

    int count = 0;
};
