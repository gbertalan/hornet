#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"
class FontAtlas;
class FontRenderer;
class BoxListPanel;
class BorderedPanel;
class QPushButton;
class QListWidget;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
struct FilePathListDTO;
class ScriptRunnerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptRunnerPanel(FontAtlas &fontAtlas,
                               FontRenderer &fontRenderer,
                               QWidget *parent = nullptr);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void refreshRunnableBoxes();
    int preferredHeight() const;
signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void boxRunRequested(int boxId);
    void runRequested(const FilePathListDTO &dto);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    BorderedPanel *m_runnableBoxesContainer;
    BoxListPanel *m_runnableBoxesList;
    BorderedPanel *m_runMoreContainer;
    QPushButton *m_browseButton;
    QListWidget *m_pendingFilesList;
    QPushButton *m_runButton;
    QString m_lastRunText;
    int m_runnableLabelY = 0;
    int m_lastRunReadoutY = 0;
    int m_runMoreLabelY = 0;
    static constexpr int m_listVisibleRows = 4;
    static constexpr int m_listShiftRight = 110;
    static constexpr int m_margin = 10;
    static constexpr int m_labelHeight = 18;
    static constexpr int m_gapLabelToList = 14;
    static constexpr int m_gapListToReadout = 20;
    static constexpr int m_readoutHeight = 14;
    static constexpr int m_gapReadoutToLabel = 30;
    static constexpr int m_gapLabelToRunMore = 14;
    static constexpr int m_runMoreHeight = 200;
};