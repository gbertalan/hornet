#include "view_layer/fileloaderpanel.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include "theme.h"
#include "view_layer/boxlistpanel.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

// ================================================================
// BorderedPanel - amber-bordered container, used for both sections
// ================================================================

BorderedPanel::BorderedPanel(QWidget *parent)
    : QWidget(parent)
{}

void BorderedPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::almostBlack());
    painter.setPen(QPen(Theme::darkAmber(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

// ================================================================
// FileLoaderPanel
// ================================================================

FileLoaderPanel::FileLoaderPanel(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    m_loadedBoxesContainer = new BorderedPanel(this);
    m_loadedBoxesList = new BoxListPanel(fontAtlas,
                                         fontRenderer,
                                         m_listVisibleRows,
                                         m_loadedBoxesContainer);
    connect(m_loadedBoxesList,
            &BoxListPanel::boxListPageRequested,
            this,
            &FileLoaderPanel::boxListPageRequested);
    connect(m_loadedBoxesList,
            &BoxListPanel::entryClicked,
            this,
            [this](const BoxListEntryDTO &entry) {
                m_selectedEntryText = "Selected: " + entry.headerText + " (#"
                                      + QString::number(entry.id) + ")";
                update();
            });

    m_loadMoreContainer = new BorderedPanel(this);

    m_browseButton = new QPushButton("Browse...", m_loadMoreContainer);
    m_pathField = new QLineEdit(m_loadMoreContainer);
    m_pathField->setPlaceholderText("path");
    m_extensionField = new QLineEdit(m_loadMoreContainer);
    m_extensionField->setPlaceholderText("cpp");
    m_recursiveCheckBox = new QCheckBox("Recursive", m_loadMoreContainer);
    m_loadButton = new QPushButton("Load", m_loadMoreContainer);

    const QString fieldStyle
        = "QLineEdit, QPushButton, QCheckBox { color: #d0d0d0; background-color: #1a1a1a; "
          "border: 1px solid #4e4c4a; }";
    m_browseButton->setStyleSheet(fieldStyle);
    m_pathField->setStyleSheet(fieldStyle);
    m_extensionField->setStyleSheet(fieldStyle);
    m_recursiveCheckBox->setStyleSheet(fieldStyle);
    m_loadButton->setStyleSheet(fieldStyle);

    QVBoxLayout *loadMoreLayout = new QVBoxLayout(m_loadMoreContainer);
    loadMoreLayout->setContentsMargins(14, 14, 14, 14);
    loadMoreLayout->setSpacing(16);

    QHBoxLayout *pathRow = new QHBoxLayout();
    pathRow->addWidget(m_browseButton);
    pathRow->addWidget(m_pathField);
    loadMoreLayout->addLayout(pathRow);

    QHBoxLayout *filterRow = new QHBoxLayout();
    filterRow->addWidget(m_extensionField);
    filterRow->addWidget(m_recursiveCheckBox);
    filterRow->addStretch();
    filterRow->addWidget(m_loadButton);
    loadMoreLayout->addLayout(filterRow);

    loadMoreLayout->addStretch();
}

void FileLoaderPanel::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_loadedBoxesList->updateBoxListPage(dto);
}

void FileLoaderPanel::refreshLoadedBoxes()
{
    m_loadedBoxesList->refresh();
}

int FileLoaderPanel::preferredHeight() const
{
    const int listHeight = m_listVisibleRows * 34 + 4;
    return m_margin + m_labelHeight + m_gapLabelToList + listHeight + m_gapListToReadout
           + m_readoutHeight + m_gapReadoutToLabel + m_labelHeight + m_gapLabelToLoadMore
           + m_loadMoreHeight + m_margin;
}

void FileLoaderPanel::layoutChildren()
{
    const int listHeight = m_listVisibleRows * 34 + 4;

    m_loadedLabelY = m_margin;
    const int listY = m_loadedLabelY + m_labelHeight + m_gapLabelToList;
    const int listX = m_margin + m_listShiftRight;
    const int listW = width() - listX - m_margin;
    m_loadedBoxesContainer->setGeometry(listX, listY, listW, listHeight);
    m_loadedBoxesList->setGeometry(2, 2, listW - 4, listHeight - 4);

    m_selectedReadoutY = listY + listHeight + m_gapListToReadout;

    m_loadMoreLabelY = m_selectedReadoutY + m_readoutHeight + m_gapReadoutToLabel;
    const int loadMoreY = m_loadMoreLabelY + m_labelHeight + m_gapLabelToLoadMore;
    m_loadMoreContainer->setGeometry(m_margin, loadMoreY, width() - 2 * m_margin, m_loadMoreHeight);
}

void FileLoaderPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float scale = 0.6f;
    m_fontRenderer
        .drawText(painter, m_margin, m_loadedLabelY, "Loaded boxes:", Theme::darkAmber(), scale);
    if (!m_selectedEntryText.isEmpty())
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_selectedReadoutY,
                                m_selectedEntryText,
                                Theme::desaturatedTeal(),
                                scale * 0.85f);
    m_fontRenderer
        .drawText(painter, m_margin, m_loadMoreLabelY, "Load more:", Theme::darkAmber(), scale);
}

void FileLoaderPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}
