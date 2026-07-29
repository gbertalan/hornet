#ifndef BOXVIEWDTO_H
#define BOXVIEWDTO_H
#include <QString>
#include <QVector>

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
               int cursorY)
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
    {}
};
#endif // BOXVIEWDTO_H
