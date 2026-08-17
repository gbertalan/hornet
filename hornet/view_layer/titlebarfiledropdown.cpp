#include "view_layer/titlebarfiledropdown.h"
#include <QPainter>
#include "shared/dto_model_to_view/boxlistentrydto.h"
#include "shared/dto_model_to_view/boxlistpagedto.h"
#include "shared/dto_model_to_view/boxlistpagerequestdto.h"
#include "theme.h"
#include "view_layer/boxlistpanel.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"

TitlebarFileDropdown::TitlebarFileDropdown(FontAtlas &fontAtlas,
                                           FontRenderer &fontRenderer,
                                           QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    setFixedWidth(m_width);

    m_listPanel = new BoxListPanel(fontAtlas, fontRenderer, 10, this);
    m_listPanel->setGeometry(1, m_topRowHeight, m_width - 2, m_listPanel->height());
    setFixedHeight(m_topRowHeight + m_listPanel->height());
    hide();

    connect(m_listPanel,
            &BoxListPanel::boxListPageRequested,
            this,
            &TitlebarFileDropdown::boxListPageRequested);
    connect(m_listPanel, &BoxListPanel::entryClicked, this, [this](const BoxListEntryDTO &entry) {
        emit boxSelected(BoxSelectedDTO(entry.id));
        m_currentFileName = entry.headerText;
        update();
    });
}

void TitlebarFileDropdown::openAt(int x, int y, const QString &currentFileName)
{
    m_currentFileName = currentFileName;
    move(x, y);
    m_listPanel->refresh();
    show();
    raise();
    update();
}

void TitlebarFileDropdown::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_listPanel->updateBoxListPage(dto);
}

void TitlebarFileDropdown::setCurrentBoxId(int boxId)
{
    m_currentBoxId = boxId;
    m_listPanel->setHighlightedBoxId(boxId);
}

void TitlebarFileDropdown::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::almostBlack());
    painter.fillRect(QRect(0, 0, m_width, m_topRowHeight), Theme::darkGray());

    const float scale = 0.7f;
    const float textVisualHeight = static_cast<float>(m_fontAtlas.getAscenderPx()
                                                      + m_fontAtlas.getDescenderPx())
                                   * scale;
    const float y = (static_cast<float>(m_topRowHeight) - textVisualHeight) / 2.0f;
    m_fontRenderer.drawText(painter, 10.f, y, m_currentFileName, Theme::darkAmber(), scale);

    painter.setPen(QPen(Theme::darkAmber(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void TitlebarFileDropdown::leaveEvent(QEvent *event)
{
    hide();
}
