#ifndef BOXVIEWDTO_H
#define BOXVIEWDTO_H
#include <QString>
#include <QVector>
#include "model_layer/boxcontenttype.h"
#include "model_layer/markrange.h"
#include "shared/dto_model_to_view/toolscriptdto.h"
struct BoxViewDTO
{
    int id;
    int posX;
    int posY;
    int width;
    int height;
    QString headerText;
    QVector<QString> bodyLines;
    int totalBodyLineCount;
    int bodyScrollOffset;
    int cursorX;
    int cursorY;
    int selectionAnchorX;
    int selectionAnchorY;
    int selectionExtentX;
    int selectionExtentY;
    bool hasSelection;
    QVector<MarkRange> marks;
    BoxContentType contentType;
    ToolScriptDTO toolScript;
    BoxViewDTO(int id,
               int posX,
               int posY,
               int width,
               int height,
               const QString &headerText,
               const QVector<QString> &bodyLines,
               int totalBodyLineCount,
               int bodyScrollOffset,
               int cursorX,
               int cursorY,
               int selectionAnchorX,
               int selectionAnchorY,
               int selectionExtentX,
               int selectionExtentY,
               bool hasSelection,
               const QVector<MarkRange> &marks,
               BoxContentType contentType,
               const ToolScriptDTO &toolScript)
        : id(id)
        , posX(posX)
        , posY(posY)
        , width(width)
        , height(height)
        , headerText(headerText)
        , bodyLines(bodyLines)
        , totalBodyLineCount(totalBodyLineCount)
        , bodyScrollOffset(bodyScrollOffset)
        , cursorX(cursorX)
        , cursorY(cursorY)
        , selectionAnchorX(selectionAnchorX)
        , selectionAnchorY(selectionAnchorY)
        , selectionExtentX(selectionExtentX)
        , selectionExtentY(selectionExtentY)
        , hasSelection(hasSelection)
        , marks(marks)
        , contentType(contentType)
        , toolScript(toolScript)
    {}
};
#endif // BOXVIEWDTO_H