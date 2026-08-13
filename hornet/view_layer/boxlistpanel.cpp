#include "view_layer/boxlistpanel.h"
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
// BoxListPanelContent - paints the scrollable rows
// ================================================================

BoxListPanelContent::BoxListPanelContent(FontAtlas &fontAtlas,
                                         FontRenderer &fontRenderer,
                                         int visibleRows,
                                         QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
    , m_visibleRows(visibleRows)
{
    setMouseTracking(true);
}

void BoxListPanelContent::setEntries(const std::vector<BoxListEntryDTO> &entries,
                                     int totalCount,
                                     int highestBoxId,
                                     int startIndex)
{
    m_entries = entries;
    m_startIndex = startIndex;
    m_highestBoxId = highestBoxId;
    const int viewportHeight = m_visibleRows * m_rowHeight;
    const int contentHeight = totalCount > 0 ? (totalCount - 1) * m_rowHeight + viewportHeight
                                             : viewportHeight;
    setFixedHeight(contentHeight);
    update();
}

void BoxListPanelContent::setHighlightedBoxId(int boxId)
{
    m_highlightedBoxId = boxId;
    update();
}

void BoxListPanelContent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float scale = 0.55f;
    const float idScale = 0.42f;
    const float textPaddingTop = 6.f;

    const qsizetype maxIdDigits = std::max(qsizetype(1), QString::number(m_highestBoxId).length());
    const float idColumnWidth = m_fontAtlas.textWidth(maxIdDigits + 1, idScale);

    for (int i = 0; i < static_cast<int>(m_entries.size()); ++i) {
        const BoxListEntryDTO &entry = m_entries.at(i);
        const int rowTop = (m_startIndex + i) * m_rowHeight;
        if (entry.id == m_highlightedBoxId) {
            painter.fillRect(QRectF(0, rowTop, width(), m_rowHeight), Theme::warmGray());
            painter.save();
            painter.setPen(QPen(Theme::darkGray(), 1));
            painter.drawLine(QPointF(0, rowTop), QPointF(width(), rowTop));
            painter.drawLine(QPointF(0, rowTop + m_rowHeight - 1),
                             QPointF(width(), rowTop + m_rowHeight));
            painter.restore();
        } else if (m_startIndex + i == m_hoveredRowIndex) {
            painter.fillRect(QRectF(0, rowTop, width(), m_rowHeight), Theme::darkerGray());
        }
        const QString idLabel = "#" + QString::number(entry.id);
        const float idY = rowTop + textPaddingTop - 5.f;
        m_fontRenderer.drawText(painter, 2.f, idY, idLabel, Theme::darkGray(), idScale);

        const float y = rowTop + textPaddingTop;
        const auto headerTextColor = (entry.id == m_highlightedBoxId)
                                         ? Theme::darkGray()
                                         : (entry.isFileBacked ? Theme::darkAmber()
                                                               : Theme::desaturatedTeal());
        m_fontRenderer
            .drawText(painter, idColumnWidth + 10.f, y, entry.headerText, headerTextColor, scale);
    }
}

void BoxListPanelContent::mouseMoveEvent(QMouseEvent *event)
{
    const int rowIndex = event->pos().y() / m_rowHeight;
    if (rowIndex != m_hoveredRowIndex) {
        m_hoveredRowIndex = rowIndex;
        update();
    }
}

void BoxListPanelContent::leaveEvent(QEvent *event)
{
    if (m_hoveredRowIndex != -1) {
        m_hoveredRowIndex = -1;
        update();
    }
}

void BoxListPanelContent::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    const int absoluteRowIndex = event->pos().y() / m_rowHeight;
    const int localIndex = absoluteRowIndex - m_startIndex;
    if (localIndex < 0 || localIndex >= static_cast<int>(m_entries.size()))
        return;
    emit entryClicked(m_entries.at(localIndex));
}

// ================================================================
// BoxListPanel - scrollable, reusable list of box entries.
// Host widgets provide their own chrome (border, width); this
// widget fills whatever rect it's given.
// ================================================================

BoxListPanel::BoxListPanel(FontAtlas &fontAtlas,
                           FontRenderer &fontRenderer,
                           int visibleRows,
                           QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
    , m_visibleRows(visibleRows)
{
    setFixedHeight(m_visibleRows * m_rowHeight);

    m_content = new BoxListPanelContent(fontAtlas, fontRenderer, visibleRows, this);
    connect(m_content, &BoxListPanelContent::entryClicked, this, &BoxListPanel::entryClicked);

    m_scrollArea = new QScrollArea(this);
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

void BoxListPanel::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_content->setEntries(dto.entries, dto.totalCount, dto.highestBoxId, dto.startIndex);
}

void BoxListPanel::setHighlightedBoxId(int boxId)
{
    m_content->setHighlightedBoxId(boxId);
}

void BoxListPanel::refresh()
{
    m_scrollArea->verticalScrollBar()->setValue(0);
    emit boxListPageRequested(BoxListPageRequestDTO{0, m_visibleRows + 1});
}

void BoxListPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_scrollArea->setGeometry(0, 0, width(), height());
    m_content->setFixedWidth(width());
}
