#include "mainpopup.h"
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include "theme.h"
#include "view_layer/customscrollbar.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

MainPopup::MainPopup(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    setFixedSize(m_width, m_height);
    setFocusPolicy(Qt::StrongFocus);
    hide();

    m_bodyContent = new QWidget(this);
    m_bodyContent->setFixedSize(m_width - 4, m_height - m_headerHeight - 2);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setGeometry(2, m_headerHeight, m_width - 4, m_height - m_headerHeight - 2);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_bodyContent);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    m_scrollArea->viewport()->setStyleSheet("background: transparent;");

    m_verticalScrollBar = new CustomScrollBar(Qt::Vertical, m_scrollArea);
    m_verticalScrollBar->setStyleSheet(
        "QScrollBar:vertical { background: transparent; width: 6px; }"
        "QScrollBar::handle:vertical { background: transparent; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; "
        "}");
    m_scrollArea->setVerticalScrollBar(m_verticalScrollBar);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollSnapTimer = new QTimer(this);
    m_scrollSnapTimer->setSingleShot(true);
    connect(m_scrollSnapTimer, &QTimer::timeout, this, [this]() {
        constexpr int snapThreshold = 40;
        const int value = m_verticalScrollBar->value();
        const int minValue = m_verticalScrollBar->minimum();
        const int maxValue = m_verticalScrollBar->maximum();
        if (value != minValue && value - minValue <= snapThreshold)
            m_verticalScrollBar->setValue(minValue);
        else if (value != maxValue && maxValue - value <= snapThreshold)
            m_verticalScrollBar->setValue(maxValue);
    });
    connect(m_verticalScrollBar, &QScrollBar::valueChanged, this, [this]() {
        m_scrollSnapTimer->start(150);
    });

    const QString closeButtonStyle
        = QString("QPushButton { color: %1; background-color: transparent; border: none; "
                  "font-size: 26px; font-weight: bold; }"
                  "QPushButton:hover { background-color: rgba(0, 0, 0, 40); }")
              .arg(Theme::almostBlack().name());
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setStyleSheet(closeButtonStyle);
    m_closeButton->move(m_width - 30 - 8, (m_headerHeight - 30) / 2);
    m_closeButton->raise();
    connect(m_closeButton, &QPushButton::clicked, this, &MainPopup::closeRequested);
}

void MainPopup::setHeaderText(const QString &text)
{
    m_headerText = text;
    update();
}

QWidget *MainPopup::bodyWidget() const
{
    return m_bodyContent;
}

void MainPopup::setBodyContentHeight(int height)
{
    m_bodyContent->setFixedHeight(std::max(height, m_height - m_headerHeight - 2));
}

void MainPopup::resetBodyContentHeight()
{
    m_bodyContent->setFixedHeight(m_height - m_headerHeight - 2);
}

void MainPopup::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::darkerGray());
    painter.fillRect(QRectF(0, 0, m_width, m_headerHeight), Theme::darkAmber());

    painter.setPen(QPen(Theme::darkAmber(), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    const float scale = 0.7f;
    const float textPadding = 10.f;
    const float availableWidth = static_cast<float>(m_width) - (textPadding * 2.0f);
    const float textWidth = m_fontAtlas.textWidth(m_headerText.length(), scale);
    const float textX = (textWidth <= availableWidth)
                            ? (static_cast<float>(m_width) - textWidth) / 2.0f
                            : textPadding;
    const int bottomPadding = 4;
    const float textY = ((static_cast<float>(m_headerHeight) - m_fontAtlas.textHeight(scale)) / 2.0f)
                        - bottomPadding;
    m_fontRenderer.drawText(painter, textX, textY, m_headerText, Theme::almostBlack(), scale);
}

void MainPopup::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        emit closeRequested();
    else
        QWidget::keyPressEvent(event);
}
