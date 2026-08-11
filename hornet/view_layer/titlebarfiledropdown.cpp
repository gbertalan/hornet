#include "view_layer/titlebarfiledropdown.h"
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>
#include "shared/dto_model_to_view/boxlistpagedto.h"
#include "shared/dto_model_to_view/boxlistpagerequestdto.h"
#include "theme.h"
#include "view_layer/customscrollbar.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

// ================================================================
// TitlebarFileDropdownContent - paints the scrollable rows
// ================================================================

TitlebarFileDropdownContent::TitlebarFileDropdownContent(FontAtlas &fontAtlas,
                                                         FontRenderer &fontRenderer,
                                                         QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{}

void TitlebarFileDropdownContent::setEntries(const std::vector<BoxListEntryDTO> &entries,
                                             int totalCount,
                                             int startIndex)
{
    m_entries = entries;
    m_startIndex = startIndex;
    setFixedHeight(std::max(totalCount, 1) * m_rowHeight);
    update();
}

void TitlebarFileDropdownContent::setCurrentBoxId(int boxId)
{
    m_currentBoxId = boxId;
    update();
}

void TitlebarFileDropdownContent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float scale = 0.45f;
    const float idScale = 0.35f;
    const float textPadding = 8.f;

    for (int i = 0; i < static_cast<int>(m_entries.size()); ++i) {
        const BoxListEntryDTO &entry = m_entries.at(i);
        const int rowTop = (m_startIndex + i) * m_rowHeight;

        if (entry.id == m_currentBoxId)
            painter.fillRect(QRectF(0, rowTop, width(), m_rowHeight), Theme::warmGray());

        const float y = rowTop + (m_rowHeight - m_fontAtlas.textHeight(scale)) / 2.f;
        m_fontRenderer.drawText(painter, textPadding, y, entry.headerText, Theme::darkAmber(), scale);

        const QString idLabel = "#" + QString::number(entry.id);
        const float idWidth = m_fontAtlas.textWidth(idLabel.length(), idScale);
        const float idX = width() - idWidth - textPadding;
        const float idY = rowTop + (m_rowHeight - m_fontAtlas.textHeight(idScale)) / 2.f;
        m_fontRenderer.drawText(painter, idX, idY, idLabel, Theme::darkGray(), idScale);
    }
}

// ================================================================
// TitlebarFileDropdown - fixed current-file row + scrollable list
// ================================================================

TitlebarFileDropdown::TitlebarFileDropdown(FontAtlas &fontAtlas,
                                           FontRenderer &fontRenderer,
                                           QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    setFixedWidth(m_width);
    setFixedHeight(m_topRowHeight + m_visibleRows * m_rowHeight);
    hide();

    m_content = new TitlebarFileDropdownContent(fontAtlas, fontRenderer, this);
    m_content->setFixedWidth(m_width);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setGeometry(0, m_topRowHeight, m_width, m_visibleRows * m_rowHeight);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_content);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    m_scrollArea->viewport()->setStyleSheet("background: transparent;");

    m_verticalScrollBar = new CustomScrollBar(Qt::Vertical, m_scrollArea);
    m_scrollArea->setVerticalScrollBar(m_verticalScrollBar);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(m_verticalScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        const int startIndex = value / m_rowHeight;
        emit boxListPageRequested(BoxListPageRequestDTO{startIndex, m_visibleRows});
    });
}

void TitlebarFileDropdown::openAt(int x, int y, const QString &currentFileName)
{
    m_currentFileName = currentFileName;
    move(x, y);
    show();
    raise();
    emit boxListPageRequested(BoxListPageRequestDTO{0, m_visibleRows});
    update();
}

void TitlebarFileDropdown::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_content->setEntries(dto.entries, dto.totalCount, dto.startIndex);
}

void TitlebarFileDropdown::setCurrentBoxId(int boxId)
{
    m_currentBoxId = boxId;
    m_content->setCurrentBoxId(boxId);
}

void TitlebarFileDropdown::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(QRect(0, 0, m_width, m_topRowHeight), Theme::warmGray());

    const float scale = 0.5f;
    const float y = m_topRowHeight - m_fontAtlas.textHeight(scale) - 6.f;
    m_fontRenderer.drawText(painter, 10.f, y, m_currentFileName, Theme::darkAmber(), scale);

    painter.setPen(QPen(Theme::darkAmber(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void TitlebarFileDropdown::leaveEvent(QEvent *event)
{
    hide();
}
