#pragma once
#include <QWidget>
#include "shared/dto_view_to_model/boxselecteddto.h"

class FontAtlas;
class FontRenderer;
class BoxListPanel;
struct BoxListPageDTO;
struct BoxListPageRequestDTO;

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

    QString m_currentFileName;
    int m_currentBoxId = -1;

    BoxListPanel *m_listPanel;

    FontAtlas &m_fontAtlas;
    FontRenderer &m_fontRenderer;
};
