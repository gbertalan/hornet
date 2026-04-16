#include "editorcontainer.h"
#include <QPainter>

EditorContainer::EditorContainer(QWidget *parent)
    : QWidget(parent)
{}

void EditorContainer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::blue);
}
