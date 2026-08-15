#ifndef RENDERTEXTDTO_H
#define RENDERTEXTDTO_H
#include <QString>
struct ToolTextDTO
{
    double x;
    double y;
    QString text;
    QString colorToken;
    ToolTextDTO(double x, double y, const QString &text, const QString &colorToken = QString())
        : x(x)
        , y(y)
        , text(text)
        , colorToken(colorToken)
    {}
};
#endif // RENDERTEXTDTO_H