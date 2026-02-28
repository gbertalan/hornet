#include "view_layer/lowerwidget.h"
#include "theme.h"
#include <QPalette>

LowerWidget::LowerWidget(QWidget* parent) : QWidget(parent) {
    QPalette palette;
    palette.setColor(QPalette::Window, Theme::desaturatedTeal());
    setPalette(palette);
    setAutoFillBackground(true);
}
