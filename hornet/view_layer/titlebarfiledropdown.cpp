#include "view_layer/titlebarfiledropdown.h"
#include <QMouseEvent>
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
{
    setMouseTracking(true);
}

void TitlebarFileDropdownContent::setEntries(const std::vector<BoxListEntryDTO> &entries,
                                             int totalCount,
                                             int startIndex)
{
    m_entries = entries;
    m_startIndex = startIndex;
    const int viewportHeight = m_visibleRows * m_rowHeight;
    const int contentHeight = totalCount > 0 ? (totalCount - 1) * m_rowHeight + viewportHeight
                                             : viewportHeight;
    setFixedHeight(contentHeight);
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

    const float scale = 0.55f;
    const float idScale = 0.42f;
    const float textPaddingLeft = 14.f;
    const float textPaddingTop = 6.f;

    for (int i = 0; i < static_cast<int>(m_entries.size()); ++i) {
        const BoxListEntryDTO &entry = m_entries.at(i);
        const int rowTop = (m_startIndex + i) * m_rowHeight;

        if (entry.id == m_currentBoxId) {
            painter.fillRect(QRectF(0, rowTop, width(), m_rowHeight), Theme::warmGray());
            painter.save();
            painter.setPen(QPen(Theme::darkGray(), 1));
            painter.drawLine(QPointF(0, rowTop), QPointF(width(), rowTop));
            painter.drawLine(QPointF(0, rowTop + m_rowHeight),
                             QPointF(width(), rowTop + m_rowHeight));
            painter.restore();
        } else if (m_startIndex + i == m_hoveredRowIndex) {
            painter.fillRect(QRectF(0, rowTop, width(), m_rowHeight), Theme::darkerGray());
        }

        const float y = rowTop + textPaddingTop;
        const auto headerTextColor = (entry.id == m_currentBoxId) ? Theme::darkGray()
                                                                  : Theme::darkAmber();
        m_fontRenderer
            .drawText(painter, textPaddingLeft, y, entry.headerText, headerTextColor, scale);

        const QString idLabel = "#" + QString::number(entry.id);
        const float idWidth = m_fontAtlas.textWidth(idLabel.length(), idScale);
        const float idX = width() - idWidth - textPaddingLeft;
        const float idY = rowTop + textPaddingTop;
        m_fontRenderer.drawText(painter, idX, idY, idLabel, Theme::darkGray(), idScale);
    }
}

void TitlebarFileDropdownContent::mouseMoveEvent(QMouseEvent *event)
{
    const int rowIndex = event->pos().y() / m_rowHeight;
    if (rowIndex != m_hoveredRowIndex) {
        m_hoveredRowIndex = rowIndex;
        update();
    }
}

void TitlebarFileDropdownContent::leaveEvent(QEvent *event)
{
    if (m_hoveredRowIndex != -1) {
        m_hoveredRowIndex = -1;
        update();
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
    m_content->setFixedWidth(m_width - 2);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setGeometry(1, m_topRowHeight, m_width - 2, m_visibleRows * m_rowHeight);
    m_scrollArea->setWidgetResizable(false);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_content);
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

    connect(m_verticalScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        const int startIndex = value / m_rowHeight;
        emit boxListPageRequested(BoxListPageRequestDTO{startIndex, m_visibleRows + 1});
    });
}

void TitlebarFileDropdown::openAt(int x, int y, const QString &currentFileName)
{
    m_currentFileName = currentFileName;
    move(x, y);
    m_scrollArea->verticalScrollBar()->setValue(0);
    show();
    raise();
    emit boxListPageRequested(BoxListPageRequestDTO{0, m_visibleRows + 1});
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
    painter.fillRect(rect(), Theme::almostBlack());
    painter.fillRect(QRect(0, 0, m_width, m_topRowHeight), Theme::darkGray());

    const float scale = 0.7f;
    const float y = static_cast<float>(m_topRowHeight) - m_fontAtlas.textHeight(scale)
                    - static_cast<float>(m_bottomPadding);
    m_fontRenderer.drawText(painter, 10.f, y, m_currentFileName, Theme::darkAmber(), scale);

    painter.setPen(QPen(Theme::darkAmber(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void TitlebarFileDropdown::leaveEvent(QEvent *event)
{
    hide();
}
