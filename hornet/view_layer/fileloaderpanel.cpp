#include "view_layer/fileloaderpanel.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include "shared/dto_view_to_model/filepathlistdto.h"
#include "theme.h"
#include "view_layer/boxlistpanel.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

// ================================================================
// BorderedPanel - amber-bordered container, used across all three
// popup panels (FileLoader, ScriptRunner, ProjectSaver)
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
    const QColor amber = Theme::darkAmber();
    const QColor teal = Theme::desaturatedTeal();

    const QString fieldStyle = "QLineEdit, QPushButton, QCheckBox, QListWidget { color: #d0d0d0; "
                               "background-color: #1a1a1a; "
                               "border: 1px solid #4e4c4a; }";
    const QString toggleStyle = QString(
                                    "QPushButton { color: #9a9a9a; background-color: #1a1a1a; "
                                    "border: 1px solid #4e4c4a; padding: 4px 14px; }"
                                    "QPushButton:checked { color: #1a1a1a; background-color: %1; "
                                    "border: 1px solid %1; font-weight: bold; }")
                                    .arg(amber.name());
    const QString noticeStyle = QString("QLabel { color: %1; }").arg(teal.name());

    // ---- Load section (top): mode toggle, browse, pending list, load ----

    m_loadSectionContainer = new BorderedPanel(this);

    m_modeFilesButton = new QPushButton("Files", m_loadSectionContainer);
    m_modeDirectoryButton = new QPushButton("Directory", m_loadSectionContainer);
    m_modeFilesButton->setCheckable(true);
    m_modeDirectoryButton->setCheckable(true);
    m_modeFilesButton->setChecked(true);
    m_modeFilesButton->setStyleSheet(toggleStyle);
    m_modeDirectoryButton->setStyleSheet(toggleStyle);
    QButtonGroup *modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addButton(m_modeFilesButton);
    modeGroup->addButton(m_modeDirectoryButton);

    m_browseButton = new QPushButton("Choose Files…", m_loadSectionContainer);
    m_pendingFilesList = new QListWidget(m_loadSectionContainer);
    m_browseButton->setStyleSheet(fieldStyle);
    m_pendingFilesList->setStyleSheet(fieldStyle);
    m_pendingFilesList->setMinimumHeight(110);

    m_directoryOptionsRow = new QWidget(m_loadSectionContainer);
    QLabel *extensionCaption = new QLabel("Extension:", m_directoryOptionsRow);
    m_extensionField = new QLineEdit(m_directoryOptionsRow);
    m_extensionField->setPlaceholderText("cpp");
    m_recursiveCheckBox = new QCheckBox("Include subdirectories", m_directoryOptionsRow);
    m_directoryNoticeLabel
        = new QLabel("Directory browsing isn't wired up yet — this previews the layout.",
                     m_directoryOptionsRow);
    extensionCaption->setStyleSheet("QLabel { color: #9a9a9a; }");
    m_extensionField->setStyleSheet(fieldStyle);
    m_recursiveCheckBox->setStyleSheet(fieldStyle);
    m_directoryNoticeLabel->setStyleSheet(noticeStyle);
    m_directoryNoticeLabel->setWordWrap(true);

    QVBoxLayout *directoryOptionsLayout = new QVBoxLayout(m_directoryOptionsRow);
    directoryOptionsLayout->setContentsMargins(0, 0, 0, 0);
    directoryOptionsLayout->setSpacing(8);
    QHBoxLayout *extensionRow = new QHBoxLayout();
    extensionRow->addWidget(extensionCaption);
    extensionRow->addWidget(m_extensionField);
    extensionRow->addWidget(m_recursiveCheckBox);
    directoryOptionsLayout->addLayout(extensionRow);
    directoryOptionsLayout->addWidget(m_directoryNoticeLabel);
    m_directoryOptionsRow->hide();

    m_loadButton = new QPushButton("Load ▸", m_loadSectionContainer);
    m_loadButton->setStyleSheet(fieldStyle);

    QVBoxLayout *loadLayout = new QVBoxLayout(m_loadSectionContainer);
    loadLayout->setContentsMargins(16, 16, 16, 16);
    loadLayout->setSpacing(12);

    QHBoxLayout *modeRow = new QHBoxLayout();
    modeRow->addWidget(m_modeFilesButton);
    modeRow->addWidget(m_modeDirectoryButton);
    modeRow->addStretch();
    loadLayout->addLayout(modeRow);

    loadLayout->addWidget(m_browseButton);
    loadLayout->addWidget(m_pendingFilesList);
    loadLayout->addWidget(m_directoryOptionsRow);

    QHBoxLayout *loadRow = new QHBoxLayout();
    loadRow->addStretch();
    loadRow->addWidget(m_loadButton);
    loadLayout->addLayout(loadRow);

    connect(m_modeDirectoryButton, &QPushButton::toggled, this, [this](bool checked) {
        m_directoryOptionsRow->setVisible(checked);
    });

    connect(m_browseButton, &QPushButton::clicked, this, [this]() {
        const QStringList selected = QFileDialog::getOpenFileNames(this, "Select files to load");
        for (const QString &path : selected)
            m_pendingFilesList->addItem(path);
    });

    connect(m_loadButton, &QPushButton::clicked, this, [this]() {
        QStringList paths;
        for (int i = 0; i < m_pendingFilesList->count(); ++i)
            paths.push_back(m_pendingFilesList->item(i)->text());
        if (paths.isEmpty())
            return;
        emit loadRequested(FilePathListDTO(paths));
        m_pendingFilesList->clear();
    });

    // ---- Loaded section (bottom): boxes currently on the grid ----

    m_loadedSectionContainer = new BorderedPanel(this);
    m_loadedBoxesList = new BoxListPanel(fontAtlas,
                                         fontRenderer,
                                         m_listVisibleRows,
                                         m_loadedSectionContainer);
    connect(m_loadedBoxesList,
            &BoxListPanel::boxListPageRequested,
            this,
            &FileLoaderPanel::boxListPageRequested);
    connect(m_loadedBoxesList,
            &BoxListPanel::entryClicked,
            this,
            [this](const BoxListEntryDTO &entry) {
                m_statusReadoutText = "→ " + entry.headerText + "  (#" + QString::number(entry.id)
                                      + ")";
                update();
            });
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
    return m_margin + m_eyebrowHeight + m_gapEyebrowToDivider + m_gapDividerToCaption
           + m_captionHeight + m_gapCaptionToContainer + m_loadContainerHeight + m_gapSection
           + m_eyebrowHeight + m_gapEyebrowToDivider + m_gapDividerToCaption + m_captionHeight
           + m_gapCaptionToContainer + listHeight + m_gapListToReadout + m_readoutHeight + m_margin;
}

void FileLoaderPanel::layoutChildren()
{
    const int listHeight = m_listVisibleRows * 34 + 4;
    const int contentWidth = width() - 2 * m_margin;

    m_loadEyebrowY = m_margin;
    m_loadDividerY = m_loadEyebrowY + m_eyebrowHeight + m_gapEyebrowToDivider;
    m_loadCaptionY = m_loadDividerY + m_gapDividerToCaption;
    const int loadContainerY = m_loadCaptionY + m_captionHeight + m_gapCaptionToContainer;
    m_loadSectionContainer->setGeometry(m_margin,
                                        loadContainerY,
                                        contentWidth,
                                        m_loadContainerHeight);

    m_loadedEyebrowY = loadContainerY + m_loadContainerHeight + m_gapSection;
    m_loadedDividerY = m_loadedEyebrowY + m_eyebrowHeight + m_gapEyebrowToDivider;
    m_loadedCaptionY = m_loadedDividerY + m_gapDividerToCaption;
    const int loadedContainerY = m_loadedCaptionY + m_captionHeight + m_gapCaptionToContainer;
    m_loadedSectionContainer->setGeometry(m_margin, loadedContainerY, contentWidth, listHeight);
    m_loadedBoxesList->setGeometry(2, 2, contentWidth - 4, listHeight - 4);

    m_statusReadoutY = loadedContainerY + listHeight + m_gapListToReadout;
}

void FileLoaderPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float eyebrowScale = 0.62f;
    const float captionScale = 0.44f;
    const float readoutScale = 0.5f;
    const int contentWidth = width() - 2 * m_margin;

    QColor dividerColor = Theme::darkAmber();
    dividerColor.setAlpha(90);
    painter.setPen(QPen(dividerColor, 1));

    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_loadEyebrowY,
                            "> LOAD SOURCE",
                            Theme::darkAmber(),
                            eyebrowScale);
    painter.drawLine(QPointF(m_margin, m_loadDividerY),
                     QPointF(m_margin + contentWidth, m_loadDividerY));
    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_loadCaptionY,
                            "Bring files onto the grid, individually or by extension.",
                            Theme::desaturatedTeal(),
                            captionScale);

    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_loadedEyebrowY,
                            "> LOADED ON GRID",
                            Theme::darkAmber(),
                            eyebrowScale);
    painter.drawLine(QPointF(m_margin, m_loadedDividerY),
                     QPointF(m_margin + contentWidth, m_loadedDividerY));
    m_fontRenderer.drawText(painter,
                            m_margin,
                            m_loadedCaptionY,
                            "Boxes currently on the grid. Unloading from here is coming soon.",
                            Theme::desaturatedTeal(),
                            captionScale);

    if (!m_statusReadoutText.isEmpty())
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_statusReadoutY,
                                m_statusReadoutText,
                                Theme::desaturatedTeal(),
                                readoutScale);
}

void FileLoaderPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}