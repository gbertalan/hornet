#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"
#include <vector>

class FontAtlas;
class FontRenderer;
class QScrollArea;
class CustomScrollBar;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;
class QEvent;

class BoxListPanelContent : public QWidget
{
    Q_OBJECT
public:
    explicit BoxListPanelContent(FontAtlas &fontAtlas,
                                 FontRenderer &fontRenderer,
                                 int visibleRows,
                                 QWidget *parent = nullptr);
    void setEntries(const std::vector<BoxListEntryDTO> &entries,
                    int totalCount,
                    int highestBoxId,
                    int startIndex);
    void setHighlightedBoxId(int boxId);

signals:
    void entryClicked(const BoxListEntryDTO &entry);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
    std::vector<BoxListEntryDTO> m_entries;
    int m_startIndex = 0;
    int m_highlightedBoxId = -1;
    static constexpr int m_rowHeight = 34;
    int m_hoveredRowIndex = -1;
    int m_visibleRows;
    int m_highestBoxId = 0;
};

class BoxListPanel : public QWidget
{
    Q_OBJECT
public:
    explicit BoxListPanel(FontAtlas &fontAtlas,
                          FontRenderer &fontRenderer,
                          int visibleRows = 10,
                          QWidget *parent = nullptr,
                          bool disableWheelScroll = false);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void setHighlightedBoxId(int boxId);
    void refresh();
signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void entryClicked(const BoxListEntryDTO &entry);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    int m_visibleRows;
    bool m_disableWheelScroll;
    static constexpr int m_rowHeight = 34;
    QScrollArea *m_scrollArea;
    CustomScrollBar *m_verticalScrollBar;
    BoxListPanelContent *m_content;
    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
};
