#include "view_layer/tooldropdownentrypanel.h"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QPushButton>
#include "theme.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"

DropdownEntryPanel::DropdownEntryPanel(FontAtlas &fontAtlas,
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
}

void DropdownEntryPanel::setFieldName(const QString &fieldName)
{
    m_fieldName = fieldName;
    update();
}

void DropdownEntryPanel::setOptions(const QStringList &options, const QString &currentValue)
{
    m_options = options;
    rebuildOptionButtons();
    layoutChildren();

    const QColor amber = Theme::darkAmber();
    const QString selectedStyle
        = QString("QPushButton { color: %1; background-color: #262626; border: 1px solid %1; "
                  "text-align: left; padding: 8px 14px; font-weight: bold; }")
              .arg(amber.name());
    const QString unselectedStyle
        = QString("QPushButton { color: #d0d0d0; background-color: #1a1a1a; border: 1px solid "
                  "#4e4c4a; text-align: left; padding: 8px 14px; }"
                  "QPushButton:hover { background-color: #262626; border: 1px solid %1; }")
              .arg(amber.name());

    for (int i = 0; i < static_cast<int>(m_optionButtons.size()); ++i)
        m_optionButtons.at(i)->setStyleSheet(m_options.at(i) == currentValue ? selectedStyle
                                                                             : unselectedStyle);
}

void DropdownEntryPanel::rebuildOptionButtons()
{
    for (QPushButton *button : m_optionButtons)
        button->deleteLater();
    m_optionButtons.clear();

    for (const QString &option : m_options) {
        QPushButton *button = new QPushButton(option, this);
        connect(button, &QPushButton::clicked, this, [this, option]() {
            emit valueCommitted(option);
        });
        m_optionButtons.push_back(button);
    }
}

int DropdownEntryPanel::preferredHeight() const
{
    const int optionsHeight = static_cast<int>(m_options.size()) * m_optionHeight
                              + std::max(0, static_cast<int>(m_options.size()) - 1)
                                    * m_gapBetweenOptions;
    return m_margin + m_eyebrowHeight + m_gapEyebrowToOptions + optionsHeight + m_margin;
}

void DropdownEntryPanel::layoutChildren()
{
    const int contentWidth = width() - 2 * m_margin;
    m_eyebrowY = m_margin;
    int optionY = m_eyebrowY + m_eyebrowHeight + m_gapEyebrowToOptions;
    for (QPushButton *button : m_optionButtons) {
        button->setGeometry(m_margin, optionY, contentWidth, m_optionHeight);
        optionY += m_optionHeight + m_gapBetweenOptions;
    }
}

void DropdownEntryPanel::paintEvent(QPaintEvent *event)
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

void DropdownEntryPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}