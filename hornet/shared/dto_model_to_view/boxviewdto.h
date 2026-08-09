#ifndef BOXVIEWDTO_H
#define BOXVIEWDTO_H
#include <QString>
#include <QVector>

#include "model_layer/boxcontenttype.h"
#include "shared/dto_model_to_view/renderlinedto.h"

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
    BoxContentType contentType;
    std::vector<RenderLineDTO> renderLines;
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
               BoxContentType contentType,
               const std::vector<RenderLineDTO> &renderLines)
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
        , contentType(contentType)
        , renderLines(renderLines)
    {}
};
#endif // BOXVIEWDTO_H
