#include "view_layer/tooltextfieldentrypanel.h"
#include <QFont>
#include <QFontDatabase>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include "theme.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"

TextFieldEntryPanel::TextFieldEntryPanel(FontAtlas &fontAtlas,
                                         FontRenderer &fontRenderer,
                                         QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    static const QString monoFamily = [] {
        const int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Bold.ttf");
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        return families.isEmpty() ? QString() : families.first();
    }();
    if (!monoFamily.isEmpty())
        setFont(QFont(monoFamily));

    const QColor amber = Theme::darkAmber();
    const QString fieldStyle
        = QString("QLineEdit { color: #d0d0d0; background-color: #1a1a1a; border: 1px solid "
                  "#4e4c4a; padding: 6px; }"
                  "QLineEdit:focus { border: 1px solid %1; }")
              .arg(amber.name());
    const QString commitButtonStyle
        = QString("QPushButton { color: %1; background-color: #1a1a1a; border: 1px solid "
                  "#4e4c4a; padding: 8px 24px; font-weight: bold; }"
                  "QPushButton:hover { background-color: #262626; border: 1px solid %1; }")
              .arg(amber.name());

    m_valueField = new QLineEdit(this);
    m_valueField->setStyleSheet(fieldStyle);

    m_commitButton = new QPushButton("Set ▸", this);
    m_commitButton->setStyleSheet(commitButtonStyle);
    m_commitButton->setFixedWidth(110);

    connect(m_valueField, &QLineEdit::returnPressed, this, [this]() {
        emit valueCommitted(m_valueField->text());
    });
    connect(m_commitButton, &QPushButton::clicked, this, [this]() {
        emit valueCommitted(m_valueField->text());
    });
}

void TextFieldEntryPanel::setFieldName(const QString &fieldName)
{
    m_fieldName = fieldName;
    update();
}

void TextFieldEntryPanel::setValue(const QString &value)
{
    m_valueField->setText(value);
    m_valueField->setFocus();
    m_valueField->selectAll();
}

int TextFieldEntryPanel::preferredHeight() const
{
    return m_margin + m_eyebrowHeight + m_gapEyebrowToField + m_fieldHeight + m_gapFieldToButton
           + m_buttonHeight + m_margin;
}

void TextFieldEntryPanel::layoutChildren()
{
    const int contentWidth = width() - 2 * m_margin;
    m_eyebrowY = m_margin;
    const int fieldY = m_eyebrowY + m_eyebrowHeight + m_gapEyebrowToField;
    m_valueField->setGeometry(m_margin, fieldY, contentWidth, m_fieldHeight);
    const int buttonY = fieldY + m_fieldHeight + m_gapFieldToButton;
    m_commitButton->setGeometry(m_margin + contentWidth - m_commitButton->width(),
                                buttonY,
                                m_commitButton->width(),
                                m_buttonHeight);
}

void TextFieldEntryPanel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    constexpr float eyebrowScale = 0.62f;
    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_eyebrowY,
                            "> SET \"" + m_fieldName + "\"",
                            Theme::darkAmber(),
                            eyebrowScale);
}

void TextFieldEntryPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}