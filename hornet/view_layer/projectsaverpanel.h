#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"
class FontAtlas;
class FontRenderer;
class BoxListPanel;
class BorderedPanel;
class QLineEdit;
class QLabel;
class QPushButton;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
class ProjectSaverPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectSaverPanel(FontAtlas &fontAtlas,
                               FontRenderer &fontRenderer,
                               QWidget *parent = nullptr);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void refreshBoxes();
    void updateSaveResult(const QString &message);
    int preferredHeight() const;
signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void saveRequested(const QString &baseName);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    BorderedPanel *m_boxesContainer;
    BoxListPanel *m_boxesList;
    BorderedPanel *m_saveContainer;
    QLineEdit *m_filenameField;
    QLabel *m_extensionLabel;
    QPushButton *m_saveButton;
    QString m_lastSaveText;
    int m_boxesLabelY = 0;
    int m_lastSaveReadoutY = 0;
    int m_saveLabelY = 0;
    static constexpr int m_listVisibleRows = 4;
    static constexpr int m_listShiftRight = 110;
    static constexpr int m_margin = 10;
    static constexpr int m_labelHeight = 18;
    static constexpr int m_gapLabelToList = 14;
    static constexpr int m_gapListToReadout = 20;
    static constexpr int m_readoutHeight = 14;
    static constexpr int m_gapReadoutToLabel = 30;
    static constexpr int m_gapLabelToSave = 14;
    static constexpr int m_saveHeight = 90;
};