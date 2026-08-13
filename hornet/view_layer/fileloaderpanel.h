#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"

class FontAtlas;
class FontRenderer;
class BoxListPanel;
class QLineEdit;
class QCheckBox;
class QPushButton;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;

class BorderedPanel : public QWidget
{
    Q_OBJECT
public:
    explicit BorderedPanel(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

class FileLoaderPanel : public QWidget
{
    Q_OBJECT
public:
    explicit FileLoaderPanel(FontAtlas &fontAtlas,
                             FontRenderer &fontRenderer,
                             QWidget *parent = nullptr);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void refreshLoadedBoxes();
    int preferredHeight() const;

signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void layoutChildren();

    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;

    BorderedPanel *m_loadedBoxesContainer;
    BoxListPanel *m_loadedBoxesList;

    BorderedPanel *m_loadMoreContainer;
    QPushButton *m_browseButton;
    QLineEdit *m_pathField;
    QLineEdit *m_extensionField;
    QCheckBox *m_recursiveCheckBox;
    QPushButton *m_loadButton;

    QString m_selectedEntryText;

    int m_loadedLabelY = 0;
    int m_selectedReadoutY = 0;
    int m_loadMoreLabelY = 0;
    static constexpr int m_listVisibleRows = 4;
    static constexpr int m_listShiftRight = 110;
    static constexpr int m_margin = 10;
    static constexpr int m_labelHeight = 18;
    static constexpr int m_gapLabelToList = 14;
    static constexpr int m_gapListToReadout = 20;
    static constexpr int m_readoutHeight = 14;
    static constexpr int m_gapReadoutToLabel = 30;
    static constexpr int m_gapLabelToLoadMore = 14;
    static constexpr int m_loadMoreHeight = 190;
};
