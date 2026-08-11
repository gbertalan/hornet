#pragma once

#include <QWidget>
#include <QPoint>

#include "view_layer/window.h"

class TitlebarButton;
class TitlebarFileNameButton;
class FontAtlas;
class FontRenderer;
struct BoxListPageDTO;

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(FontAtlas &fontAtlas, FontRenderer &fontRenderer, QWidget *parent = nullptr);
    void setFullscreen(bool fullscreen);
    void updateFileName(const QString &fileName);
    void updateFileNameButtonPosition(int leftPaneWidth);
    QString currentFileName() const;
    QPoint fileNameDropdownAnchor() const;
    void updateCurrentBoxId(int boxId);

signals:
    void windowCloseClicked();
    void minimizeClicked();
    void maximizeClicked();
    void fileNameButtonClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_dragging;
    bool m_doubleclicked;
    double m_grabRatio;
    QPoint m_dragStartPosition;
    Window* m_window;
    TitlebarButton* m_closeButton;
    TitlebarButton* m_maxMinButton;
    TitlebarButton* m_trayButton;
    TitlebarFileNameButton *m_fileNameButton;
    QString m_currentFileName;
    int m_currentBoxId = -1;
};
