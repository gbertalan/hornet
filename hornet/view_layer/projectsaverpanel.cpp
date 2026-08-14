#include "view_layer/projectsaverpanel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include "theme.h"
#include "view_layer/boxlistpanel.h"
#include "view_layer/fileloaderpanel.h"
#include "view_layer/font_renderer/FontAtlas.h"
#include "view_layer/font_renderer/FontRenderer.h"
#include <algorithm>

ProjectSaverPanel::ProjectSaverPanel(FontAtlas &fontAtlas,
                                     FontRenderer &fontRenderer,
                                     QWidget *parent)
    : QWidget(parent)
    , m_fontAtlas(fontAtlas)
    , m_fontRenderer(fontRenderer)
{
    m_boxesContainer = new BorderedPanel(this);
    m_boxesList = new BoxListPanel(fontAtlas, fontRenderer, m_listVisibleRows, m_boxesContainer);
    connect(m_boxesList,
            &BoxListPanel::boxListPageRequested,
            this,
            &ProjectSaverPanel::boxListPageRequested);

    m_saveContainer = new BorderedPanel(this);

    m_filenameField = new QLineEdit("myproject", m_saveContainer);
    m_extensionLabel = new QLabel(".script", m_saveContainer);
    m_saveButton = new QPushButton("Save", m_saveContainer);

    const QString fieldStyle = "QLineEdit, QPushButton { color: #d0d0d0; "
                               "background-color: #1a1a1a; "
                               "border: 1px solid #4e4c4a; }";
    m_filenameField->setStyleSheet(fieldStyle);
    m_saveButton->setStyleSheet(fieldStyle);
    m_extensionLabel->setStyleSheet("color: #d0d0d0;");

    QVBoxLayout *saveLayout = new QVBoxLayout(m_saveContainer);
    saveLayout->setContentsMargins(14, 14, 14, 14);
    saveLayout->setSpacing(16);

    QHBoxLayout *filenameRow = new QHBoxLayout();
    filenameRow->addWidget(m_filenameField);
    filenameRow->addWidget(m_extensionLabel);
    saveLayout->addLayout(filenameRow);

    QHBoxLayout *saveRow = new QHBoxLayout();
    saveRow->addStretch();
    saveRow->addWidget(m_saveButton);
    saveLayout->addLayout(saveRow);

    connect(m_saveButton, &QPushButton::clicked, this, [this]() {
        const QString baseName = m_filenameField->text().trimmed();
        if (baseName.isEmpty())
            return;
        emit saveRequested(baseName);
    });
}

void ProjectSaverPanel::updateBoxListPage(const BoxListPageDTO &dto)
{
    m_boxesList->updateBoxListPage(dto);
}

void ProjectSaverPanel::refreshBoxes()
{
    m_boxesList->refresh();
}

void ProjectSaverPanel::updateSaveResult(const QString &message)
{
    m_lastSaveText = message;
    update();
}

int ProjectSaverPanel::preferredHeight() const
{
    const int listHeight = m_listVisibleRows * 34 + 4;
    return m_margin + m_labelHeight + m_gapLabelToList + listHeight + m_gapListToReadout
           + m_readoutHeight + m_gapReadoutToLabel + m_labelHeight + m_gapLabelToSave + m_saveHeight
           + m_margin;
}

void ProjectSaverPanel::layoutChildren()
{
    const int listHeight = m_listVisibleRows * 34 + 4;

    m_boxesLabelY = m_margin;
    const int listY = m_boxesLabelY + m_labelHeight + m_gapLabelToList;
    const int listX = m_margin + m_listShiftRight;
    const int listW = width() - listX - m_margin;
    m_boxesContainer->setGeometry(listX, listY, listW, listHeight);
    m_boxesList->setGeometry(2, 2, listW - 4, listHeight - 4);

    m_lastSaveReadoutY = listY + listHeight + m_gapListToReadout;

    m_saveLabelY = m_lastSaveReadoutY + m_readoutHeight + m_gapReadoutToLabel;
    const int saveY = m_saveLabelY + m_labelHeight + m_gapLabelToSave;
    m_saveContainer->setGeometry(m_margin, saveY, width() - 2 * m_margin, m_saveHeight);
}

void ProjectSaverPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const float scale = 0.6f;
    m_fontRenderer.drawText(painter, m_margin, m_boxesLabelY, "Boxes:", Theme::darkAmber(), scale);
    if (!m_lastSaveText.isEmpty())
        m_fontRenderer.drawText(painter,
                                m_margin,
                                m_lastSaveReadoutY,
                                m_lastSaveText,
                                Theme::desaturatedTeal(),
                                scale * 0.85f);
    m_fontRenderer.drawText(painter, m_margin, m_saveLabelY, "Save as:", Theme::darkAmber(), scale);
}

void ProjectSaverPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}