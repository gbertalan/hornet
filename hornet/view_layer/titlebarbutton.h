#pragma once

#include <QWidget>
#include <QPixmap>
#include <QEnterEvent>

enum class TitlebarButtonType { Close, Minimize, Maximize, Tray, FileLoad, ProjectSave, ScriptRun };

class TitlebarButton : public QWidget {
    Q_OBJECT

public:
    explicit TitlebarButton(TitlebarButtonType type, QWidget* parent = nullptr);
    void setType(TitlebarButtonType type);
    void setHoverColor(const QColor& color);
    void setRightPadding(int padding);
    void setLeftPadding(int padding);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    TitlebarButtonType m_type;
    QPixmap m_pixmap;
    bool m_hovered;
    bool m_pressed;
    QColor m_hoverColor;
    int m_rightPadding;
    int m_leftPadding;

    QString iconPath() const;
};
