#pragma once

#include <QWidget>
#include <QPixmap>

enum class TitlebarButtonType {
    Close,
    Minimize,
    Maximize,
    Tray
};

class TitlebarButton : public QWidget {
    Q_OBJECT

public:
    explicit TitlebarButton(TitlebarButtonType type, QWidget* parent = nullptr);
    void setHoverColor(const QColor& color);
    void setRightPadding(int padding);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
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

    QString iconPath() const;
};
