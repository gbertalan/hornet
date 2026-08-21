#ifndef BOXCONTENTDTO_H
#define BOXCONTENTDTO_H
#include <QString>
#include <QVector>
#include "model_layer/boxcontenttype.h"
#include "model_layer/markrange.h"
struct BoxContentDTO
{
    QString headerText;
    QVector<QString> bodyLines;
    BoxContentType contentType;
    int cursorX;
    int cursorY;
    int selectionAnchorX;
    int selectionAnchorY;
    int selectionExtentX;
    int selectionExtentY;
    bool hasSelection;
    QVector<MarkRange> marks;
};
#endif // BOXCONTENTDTO_H