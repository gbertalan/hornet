#pragma once
#include <QStringList>
#include <QWidget>
#include <vector>
class FontAtlas;
class FontRenderer;
class QPushButton;
class DropdownEntryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DropdownEntryPanel(FontAtlas &fontAtlas,
                                FontRenderer &fontRenderer,
                                QWidget *parent = nullptr);
    void setFieldName(const QString &fieldName);
    void setOptions(const QStringList &options, const QString &currentValue);
    int preferredHeight() const;
signals:
    void valueCommitted(const QString &value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    void rebuildOptionButtons();
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    QString m_fieldName;
    QStringList m_options;
    std::vector<QPushButton *> m_optionButtons;
    int m_eyebrowY = 0;
    static constexpr int m_margin = 18;
    static constexpr int m_eyebrowHeight = 20;
    static constexpr int m_gapEyebrowToOptions = 16;
    static constexpr int m_optionHeight = 34;
    static constexpr int m_gapBetweenOptions = 8;
};