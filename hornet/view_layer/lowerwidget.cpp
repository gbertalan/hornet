#include "view_layer/lowerwidget.h"
#include "theme.h"
#include <QPalette>

LowerWidget::LowerWidget(QWidget* parent) : QWidget(parent) {
    QPalette palette;
    palette.setColor(QPalette::Window, Theme::almostBlack());
    setPalette(palette);
    setAutoFillBackground(true);
}
