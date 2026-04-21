#pragma once
#include <QWidget>
#include <memory>
#include <view_layer/font_renderer/FontAtlas.h>
#include <view_layer/font_renderer/FontRenderer.h>

class FontRenderer;
struct EditorVisibleLinesDTO;
struct EditorSettingsDTO;
struct EditorTextContentsDTO;

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(const EditorSettingsDTO &settings, QWidget *parent = nullptr);

    void updateWidth(int width);
    void updateHeight(int height);
    void setSettings(const EditorSettingsDTO &settings);
    void updateLines(const EditorTextContentsDTO &dto);

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

    int m_lineHeight;
    float m_fontScale;

    int m_contentWidth = 0;
    int m_contentHeight = 0;

    int m_noOfVisibleLines = 0;
    int m_topLineIndex = 0;

    QString m_fileType;
    QVector<QString> m_textLinesToDisplay;

    // int count = 0;
};
