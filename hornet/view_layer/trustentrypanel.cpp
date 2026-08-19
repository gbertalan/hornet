#include "view_layer/trustentrypanel.h"
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QPushButton>
#include "theme.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"

TrustEntryPanel::TrustEntryPanel(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent)
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
    const QString buttonStyle
        = QString("QPushButton { color: %1; background-color: #1a1a1a; border: 1px solid "
                  "#4e4c4a; padding: 8px 24px; font-weight: bold; }"
                  "QPushButton:hover { background-color: #262626; border: 1px solid %1; }")
              .arg(amber.name());
    m_trustAllButton = new QPushButton("Trust All ▸", this);
    m_trustAllButton->setStyleSheet(buttonStyle);
    m_trustAllButton->setFixedWidth(150);
    connect(m_trustAllButton, &QPushButton::clicked, this, &TrustEntryPanel::trustAllRequested);
}

void TrustEntryPanel::setCommands(const QStringList &buttonCommands,
                                  const QStringList &sourceCommands)
{
    m_buttonCommands = buttonCommands;
    m_sourceCommands = sourceCommands;
    update();
}

int TrustEntryPanel::preferredHeight() const
{
    const int totalLines = std::max<int>(1, m_buttonCommands.size() + m_sourceCommands.size());
    return m_margin + m_eyebrowHeight + m_gapEyebrowToList + (totalLines * m_lineHeight)
           + m_gapListToButton + m_buttonHeight + m_margin;
}

void TrustEntryPanel::layoutChildren()
{
    const int contentWidth = width() - 2 * m_margin;
    m_eyebrowY = m_margin;
    m_listY = m_eyebrowY + m_eyebrowHeight + m_gapEyebrowToList;
    const int totalLines = std::max<int>(1, m_buttonCommands.size() + m_sourceCommands.size());
    const int listHeight = totalLines * m_lineHeight;
    const int buttonY = m_listY + listHeight + m_gapListToButton;
    m_trustAllButton->setGeometry(m_margin + contentWidth - m_trustAllButton->width(),
                                  buttonY,
                                  m_trustAllButton->width(),
                                  m_buttonHeight);
}

void TrustEntryPanel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    constexpr float eyebrowScale = 0.62f;
    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_eyebrowY,
                            "> UNTRUSTED COMMANDS IN THIS BOX",
                            Theme::darkAmber(),
                            eyebrowScale);

    constexpr float listScale = 0.55f;
    if (m_buttonCommands.isEmpty() && m_sourceCommands.isEmpty()) {
        m_fontRenderer
            .drawText(painter, m_margin, m_listY, "  (none)", Theme::almostWhite(), listScale);
        return;
    }
    int lineIndex = 0;
    for (const QString &command : m_buttonCommands) {
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_listY + lineIndex * m_lineHeight,
                                "  " + command,
                                Theme::almostWhite(),
                                listScale);
        ++lineIndex;
    }
    for (const QString &command : m_sourceCommands) {
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_listY + lineIndex * m_lineHeight,
                                "  " + command,
                                Theme::desaturatedTeal(),
                                listScale);
        ++lineIndex;
    }
}

void TrustEntryPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}