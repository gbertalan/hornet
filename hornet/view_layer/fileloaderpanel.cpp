#include "view_layer/fileloaderpanel.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
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
    static const QString monoFamily = [] {
        const int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono-Bold.ttf");
        const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
        return families.isEmpty() ? QString() : families.first();
    }();
    m_monoFamily = monoFamily;
    if (!monoFamily.isEmpty())
        setFont(QFont(monoFamily));

    const QColor amber = Theme::darkAmber();

    const QString fieldStyle
        = QString("QLineEdit, QPushButton, QCheckBox, QListWidget { color: #d0d0d0; "
                  "background-color: #1a1a1a; border: 1px solid #4e4c4a; }"
                  "QPushButton:hover { background-color: #262626; border: 1px solid %1; }"
                  "QCheckBox:hover { border: 1px solid %1; }"
                  "QListWidget::item:hover { background-color: #262626; }")
              .arg(amber.name());
    const QString toggleStyle
        = QString("QPushButton { color: #9a9a9a; background-color: #1a1a1a; "
                  "border: 1px solid #4e4c4a; padding: 4px 14px; }"
                  "QPushButton:hover:!checked { border: 1px solid %1; color: #d0d0d0; }"
                  "QPushButton:checked { color: #1a1a1a; background-color: %1; "
                  "border: 1px solid %1; font-weight: bold; }")
              .arg(amber.name());
    const QString noticeStyle = QString("QLabel { color: %1; }").arg(Theme::darkGray().name());
    const QString browseButtonStyle
        = QString("QPushButton { color: %1; background-color: #1a1a1a; border: 1px solid #4e4c4a; }"
                  "QPushButton:hover { background-color: #262626; border: 1px solid %1; }")
              .arg(amber.name());

    // ---- Load section (top): mode toggle, browse, pending list, load ----

    m_loadSectionContainer = new BorderedPanel(this);

    m_modeFilesButton = new QPushButton("Files", m_loadSectionContainer);
    m_modeDirectoryButton = new QPushButton("Directory", m_loadSectionContainer);
    m_modeFilesButton->setCheckable(true);
    m_modeDirectoryButton->setCheckable(true);
    m_modeFilesButton->setChecked(true);
    m_modeFilesButton->setStyleSheet(toggleStyle);
    m_modeDirectoryButton->setStyleSheet(toggleStyle);
    m_modeFilesButton->setFixedWidth(110);
    m_modeDirectoryButton->setFixedWidth(110);
    QButtonGroup *modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    modeGroup->addButton(m_modeFilesButton);
    modeGroup->addButton(m_modeDirectoryButton);

    m_browseButton = new QPushButton("Choose Files…", m_loadSectionContainer);
    m_pendingFilesList = new QListWidget(m_loadSectionContainer);
    m_browseButton->setStyleSheet(browseButtonStyle);
    m_pendingFilesList->setStyleSheet(fieldStyle);
    m_pendingFilesList->setMinimumHeight(110);
    m_pendingFilesList->setTextElideMode(Qt::ElideNone);
    m_pendingFilesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(m_pendingFilesList->horizontalScrollBar(),
            &QScrollBar::rangeChanged,
            this,
            [this](int, int max) { m_pendingFilesList->horizontalScrollBar()->setValue(max); });

    m_directoryOptionsRow = new QWidget(m_loadSectionContainer);
    QLabel *extensionCaption = new QLabel("Extension:", m_directoryOptionsRow);
    m_extensionField = new QLineEdit(m_directoryOptionsRow);
    m_extensionField->setPlaceholderText("cpp");
    m_recursiveCheckBox = new QCheckBox("Include subdirectories", m_directoryOptionsRow);
    extensionCaption->setStyleSheet("QLabel { color: #9a9a9a; }");
    m_extensionField->setStyleSheet(fieldStyle);
    m_recursiveCheckBox->setStyleSheet(fieldStyle);

    QVBoxLayout *directoryOptionsLayout = new QVBoxLayout(m_directoryOptionsRow);
    directoryOptionsLayout->setContentsMargins(0, 0, 0, 0);
    directoryOptionsLayout->setSpacing(8);
    QHBoxLayout *extensionRow = new QHBoxLayout();
    extensionRow->addWidget(extensionCaption);
    extensionRow->addWidget(m_extensionField);
    extensionRow->addWidget(m_recursiveCheckBox);
    directoryOptionsLayout->addLayout(extensionRow);
    m_directoryOptionsRow->hide();

    const QString actionButtonStyleTemplate = QString(
        "QPushButton { color: %1; background-color: #1a1a1a; border: 1px solid #4e4c4a; "
        "padding: 8px 24px; font-weight: bold; }"
        "QPushButton:hover { background-color: #262626; border: 1px solid %2; }");
    const QString loadButtonStyle = actionButtonStyleTemplate.arg(amber.name(), amber.name());
    const QString clearButtonStyle = actionButtonStyleTemplate.arg(Theme::desaturatedTeal().name(),
                                                                   amber.name());

    m_clearButton = new QPushButton("Clear", m_loadSectionContainer);
    m_loadButton = new QPushButton("Load ▸", m_loadSectionContainer);
    m_clearButton->setStyleSheet(clearButtonStyle);
    m_loadButton->setStyleSheet(loadButtonStyle);
    m_clearButton->setFixedWidth(110);
    m_loadButton->setFixedWidth(110);

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
    loadRow->addWidget(m_clearButton);
    loadRow->addWidget(m_loadButton);
    loadLayout->addLayout(loadRow);

    connect(m_modeDirectoryButton, &QPushButton::toggled, this, [this](bool checked) {
        m_directoryOptionsRow->setVisible(checked);
        m_browseButton->setText(checked ? "Choose Directories…" : "Choose Files…");
    });

    connect(m_browseButton, &QPushButton::clicked, this, [this, amber]() {
        const QString fontRule = m_monoFamily.isEmpty()
                                     ? QString()
                                     : QString("font-family: '%1';").arg(m_monoFamily);
        const QString dialogStyle
            = QString("QDialog, QFileDialog { background-color: %1; color: %2; border: 2px solid "
                      "%2; %5 }"
                      "QListView, QTreeView { background-color: %6; color: %2; border: 1px solid "
                      "%3; %5 }"
                      "QListView::item:selected, QTreeView::item:selected { background-color: %2; "
                      "color: %1; }"
                      "QLineEdit, QComboBox { background-color: %1; color: %2; border: 1px solid "
                      "%3; %5 }"
                      "QPushButton { background-color: %1; color: %2; border: 1px solid %3; "
                      "padding: 4px 12px; %5 }"
                      "QPushButton:hover { border: 1px solid %2; }"
                      "QToolButton { background-color: %1; color: %2; border: none; }"
                      "QHeaderView::section { background-color: %1; color: %2; border: 1px solid "
                      "%3; %5 }"
                      "QMenu { background-color: %1; color: %2; border: 1px solid %3; %5 }"
                      "QLabel { color: %4; %5 }"
                      "QToolTip { background-color: %1; color: %4; border: 1px solid %3; %5 }")
                  .arg(Theme::darkerGray().name())
                  .arg(amber.name())
                  .arg("#4e4c4a")
                  .arg(Theme::desaturatedTeal().name())
                  .arg(fontRule)
                  .arg(Theme::almostBlack().name());

        const auto alreadyPending = [this](const QString &path) {
            for (int i = 0; i < m_pendingFilesList->count(); ++i)
                if (m_pendingFilesList->item(i)->text() == path)
                    return true;
            return false;
        };

        if (m_modeDirectoryButton->isChecked()) {
            QFileDialog dialog(this, "Select directory to load");
            dialog.setOption(QFileDialog::DontUseNativeDialog, true);
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setOption(QFileDialog::ShowDirsOnly, true);
            dialog.setLabelText(QFileDialog::Accept, "Select Directory");
            dialog.setWindowFlags(dialog.windowFlags() | Qt::FramelessWindowHint);
            for (QPushButton *dialogButton : dialog.findChildren<QPushButton *>())
                dialogButton->setIcon(QIcon());
            if (!m_monoFamily.isEmpty())
                dialog.setFont(QFont(m_monoFamily));
            dialog.resize(800, 600);
            dialog.setStyleSheet(dialogStyle);
            if (dialog.exec() == QDialog::Accepted) {
                const QStringList selected = dialog.selectedFiles();
                if (!selected.isEmpty() && !alreadyPending(selected.first()))
                    m_pendingFilesList->addItem(selected.first());
            }
        } else {
            QFileDialog dialog(this, "Select files to load");
            dialog.setOption(QFileDialog::DontUseNativeDialog, true);
            dialog.setFileMode(QFileDialog::ExistingFiles);
            dialog.setLabelText(QFileDialog::Accept, "Select File(s)");
            dialog.setWindowFlags(dialog.windowFlags() | Qt::FramelessWindowHint);
            for (QPushButton *dialogButton : dialog.findChildren<QPushButton *>())
                dialogButton->setIcon(QIcon());
            if (!m_monoFamily.isEmpty())
                dialog.setFont(QFont(m_monoFamily));
            dialog.resize(800, 600);
            dialog.setStyleSheet(dialogStyle);
            if (dialog.exec() == QDialog::Accepted) {
                const QStringList selected = dialog.selectedFiles();
                for (const QString &path : selected)
                    if (!alreadyPending(path))
                        m_pendingFilesList->addItem(path);
            }
        }
    });

    connect(m_clearButton, &QPushButton::clicked, this, [this]() { m_pendingFilesList->clear(); });

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
    m_unloadHistoryContainer = new UnloadHistoryPanel(this);
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

    m_loadedListX = m_margin;
    m_loadedListW = static_cast<int>((contentWidth - m_columnGap) * 0.42f);
    m_unloadHistoryX = m_loadedListX + m_loadedListW + m_columnGap;
    m_unloadHistoryW = width() - m_margin - m_unloadHistoryX;

    m_loadedSectionContainer->setGeometry(m_loadedListX,
                                          loadedContainerY,
                                          m_loadedListW,
                                          listHeight);
    m_loadedBoxesList->setGeometry(2, 2, m_loadedListW - 4, listHeight - 4);

    m_unloadHistoryContainer->setGeometry(m_unloadHistoryX,
                                          loadedContainerY,
                                          m_unloadHistoryW,
                                          listHeight);

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
                            "> LOAD FILE(S)",
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
                            "> UNLOAD FILE(S)",
                            Theme::darkAmber(),
                            eyebrowScale);
    painter.drawLine(QPointF(m_margin, m_loadedDividerY),
                     QPointF(m_margin + contentWidth, m_loadedDividerY));
    m_fontRenderer.drawText(painter,
                            m_loadedListX,
                            m_loadedCaptionY,
                            "Click to unload.",
                            Theme::desaturatedTeal(),
                            captionScale);
    m_fontRenderer.drawText(painter,
                            m_unloadHistoryX,
                            m_loadedCaptionY,
                            "Last unloaded",
                            Theme::desaturatedTeal(),
                            captionScale);

    if (!m_statusReadoutText.isEmpty())
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_statusReadoutY,
                                m_statusReadoutText,
                                Theme::darkGray(),
                                readoutScale);
}

// ================================================================
// UnloadHistoryPanel - teal-bordered container previewing the
// upcoming "last unloaded" list (FileLoaderPanel only, for now)
// ================================================================

UnloadHistoryPanel::UnloadHistoryPanel(QWidget *parent)
    : QWidget(parent)
{
    QLabel *placeholder = new QLabel("No unloads yet.", this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setWordWrap(true);
    placeholder->setStyleSheet(
        QString("QLabel { color: %1; border: none; background: transparent; }")
            .arg(Theme::darkGray().name()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(placeholder);
}

void UnloadHistoryPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Theme::almostBlack());
    painter.setPen(QPen(Theme::desaturatedTeal(), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void FileLoaderPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}