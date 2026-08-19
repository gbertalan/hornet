#pragma once
#include <QStringList>
#include <QWidget>
class FontAtlas;
class FontRenderer;
class QPushButton;
class TrustEntryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TrustEntryPanel(FontAtlas &fontAtlas,
                             FontRenderer &fontRenderer,
                             QWidget *parent = nullptr);
    void setCommands(const QStringList &commands);
    int preferredHeight() const;
signals:
    void trustAllRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    QStringList m_commands;
    QPushButton *m_trustAllButton;
    int m_eyebrowY = 0;
    int m_listY = 0;
    static constexpr int m_margin = 18;
    static constexpr int m_eyebrowHeight = 20;
    static constexpr int m_gapEyebrowToList = 16;
    static constexpr int m_lineHeight = 22;
    static constexpr int m_gapListToButton = 16;
    static constexpr int m_buttonHeight = 34;
};