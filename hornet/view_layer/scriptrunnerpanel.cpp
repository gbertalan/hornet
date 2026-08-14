#include "view_layer/scriptrunnerpanel.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include "shared/dto_view_to_model/filepathlistdto.h"
#include "theme.h"
#include "view_layer/boxlistpanel.h"
#include "view_layer/fileloaderpanel.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

ScriptRunnerPanel::ScriptRunnerPanel(FontAtlas &fontAtlas,
                                     FontRenderer &fontRenderer,
                                     QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    m_runnableBoxesContainer = new BorderedPanel(this);
    m_runnableBoxesList = new BoxListPanel(fontAtlas,
                                           fontRenderer,
                                           m_listVisibleRows,
                                           m_runnableBoxesContainer);
    connect(m_runnableBoxesList,
            &BoxListPanel::boxListPageRequested,
            this,
            &ScriptRunnerPanel::boxListPageRequested);
    connect(m_runnableBoxesList,
            &BoxListPanel::entryClicked,
            this,
            [this](const BoxListEntryDTO &entry) {
                m_lastRunText = "Ran: " + entry.headerText + " (#" + QString::number(entry.id)
                                + ")";
                update();
                emit boxRunRequested(entry.id);
            });

    m_runMoreContainer = new BorderedPanel(this);

    m_browseButton = new QPushButton("Browse...", m_runMoreContainer);
    m_pendingFilesList = new QListWidget(m_runMoreContainer);
    m_runButton = new QPushButton("Run", m_runMoreContainer);

    const QString fieldStyle = "QPushButton, QListWidget { color: #d0d0d0; "
                               "background-color: #1a1a1a; "
                               "border: 1px solid #4e4c4a; }";
    m_browseButton->setStyleSheet(fieldStyle);
    m_pendingFilesList->setStyleSheet(fieldStyle);
    m_runButton->setStyleSheet(fieldStyle);
    m_pendingFilesList->setMinimumHeight(90);

    QVBoxLayout *runMoreLayout = new QVBoxLayout(m_runMoreContainer);
    runMoreLayout->setContentsMargins(14, 14, 14, 14);
    runMoreLayout->setSpacing(16);

    runMoreLayout->addWidget(m_browseButton);
    runMoreLayout->addWidget(m_pendingFilesList);

    QHBoxLayout *runRow = new QHBoxLayout();
    runRow->addStretch();
    runRow->addWidget(m_runButton);
    runMoreLayout->addLayout(runRow);

    connect(m_browseButton, &QPushButton::clicked, this, [this]() {
        const QStringList selected = QFileDialog::getOpenFileNames(this, "Select scripts to run");
        for (const QString &path : selected)
            m_pendingFilesList->addItem(path);
    });

    connect(m_runButton, &QPushButton::clicked, this, [this]() {
        QStringList paths;
        for (int i = 0; i < m_pendingFilesList->count(); ++i)
            paths.push_back(m_pendingFilesList->item(i)->text());
        if (paths.isEmpty())
            return;
        emit runRequested(FilePathListDTO(paths));
        m_pendingFilesList->clear();
    });
}

void ScriptRunnerPanel::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_runnableBoxesList->updateBoxListPage(dto);
}

void ScriptRunnerPanel::refreshRunnableBoxes()
{
    m_runnableBoxesList->refresh();
}

int ScriptRunnerPanel::preferredHeight() const
{
    const int listHeight = m_listVisibleRows * 34 + 4;
    return m_margin + m_labelHeight + m_gapLabelToList + listHeight + m_gapListToReadout
           + m_readoutHeight + m_gapReadoutToLabel + m_labelHeight + m_gapLabelToRunMore
           + m_runMoreHeight + m_margin;
}

void ScriptRunnerPanel::layoutChildren()
{
    const int listHeight = m_listVisibleRows * 34 + 4;

    m_runMoreLabelY = m_margin;
    const int runMoreY = m_runMoreLabelY + m_labelHeight + m_gapLabelToRunMore;
    m_runMoreContainer->setGeometry(m_margin, runMoreY, width() - 2 * m_margin, m_runMoreHeight);

    m_runnableLabelY = runMoreY + m_runMoreHeight + m_gapReadoutToLabel;
    const int listY = m_runnableLabelY + m_labelHeight + m_gapLabelToList;
    const int listX = m_margin + m_listShiftRight;
    const int listW = width() - listX - m_margin;
    m_runnableBoxesContainer->setGeometry(listX, listY, listW, listHeight);
    m_runnableBoxesList->setGeometry(2, 2, listW - 4, listHeight - 4);

    m_lastRunReadoutY = listY + listHeight + m_gapListToReadout;
}

void ScriptRunnerPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float scale = 0.6f;
    m_fontRenderer.drawText(painter, m_margin, m_runnableLabelY, "Boxes:", Theme::darkAmber(), scale);
    if (!m_lastRunText.isEmpty())
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_lastRunReadoutY,
                                m_lastRunText,
                                Theme::desaturatedTeal(),
                                scale * 0.85f);
    m_fontRenderer
        .drawText(painter, m_margin, m_runMoreLabelY, "Run more:", Theme::darkAmber(), scale);
}

void ScriptRunnerPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}