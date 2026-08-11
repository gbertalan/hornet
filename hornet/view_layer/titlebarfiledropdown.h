#pragma once
#include <QWidget>
#include "shared/dto_model_to_view/boxlistentrydto.h"
#include "shared/dto_view_to_model/boxselecteddto.h"
#include <vector>

class FontAtlas;
class FontRenderer;
class QScrollArea;
class CustomScrollBar;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;

class TitlebarFileDropdownContent : public QWidget
{
    Q_OBJECT
public:
    explicit TitlebarFileDropdownContent(FontAtlas &fontAtlas,
                                         FontRenderer &fontRenderer,
                                         QWidget *parent = nullptr);
    void setEntries(const std::vector<BoxListEntryDTO> &entries, int totalCount, int startIndex);
    void setCurrentBoxId(int boxId);

signals:
    void boxSelected(const BoxSelectedDTO &dto);

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
    int m_currentBoxId = -1;
    static constexpr int m_rowHeight = 34;
    int m_hoveredRowIndex = -1;
    static constexpr int m_visibleRows = 10;
};

class TitlebarFileDropdown : public QWidget
{
    Q_OBJECT
public:
    explicit TitlebarFileDropdown(FontAtlas &fontAtlas,
                                  FontRenderer &fontRenderer,
                                  QWidget *parent = nullptr);
    void openAt(int x, int y, const QString &currentFileName);
    void updateBoxListPage(const BoxListPageDTO &dto);
    void setCurrentBoxId(int boxId);

signals:
    void boxListPageRequested(const BoxListPageRequestDTO &dto);
    void boxSelected(const BoxSelectedDTO &dto);

protected:
    void paintEvent(QPaintEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    static constexpr int m_width = 320;
    static constexpr int m_topRowHeight = 38;
    static constexpr int m_visibleRows = 10;
    static constexpr int m_rowHeight = 34;
    static constexpr int m_bottomPadding = 10;

    QString m_currentFileName;
    int m_currentBoxId = -1;

    QScrollArea *m_scrollArea;
    CustomScrollBar *m_verticalScrollBar;
    TitlebarFileDropdownContent *m_content;

    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
};
