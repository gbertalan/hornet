#pragma once

#include <QScrollBar>

class CustomScrollBar : public QScrollBar {
    Q_OBJECT

public:
    explicit CustomScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    bool m_hovered;
};
