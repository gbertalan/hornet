#pragma once
#include <QWidget>
class FontAtlas;
class FontRenderer;
class QLineEdit;
class QPushButton;
class TextFieldEntryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TextFieldEntryPanel(FontAtlas &fontAtlas,
                                 FontRenderer &fontRenderer,
                                 QWidget *parent = nullptr);
    void setFieldName(const QString &fieldName);
    void setValue(const QString &value);
    int preferredHeight() const;
signals:
    void valueCommitted(const QString &value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    QString m_fieldName;
    QLineEdit *m_valueField;
    QPushButton *m_commitButton;
    int m_eyebrowY = 0;
    static constexpr int m_margin = 18;
    static constexpr int m_eyebrowHeight = 20;
    static constexpr int m_gapEyebrowToField = 16;
    static constexpr int m_fieldHeight = 34;
    static constexpr int m_gapFieldToButton = 16;
    static constexpr int m_buttonHeight = 34;
};