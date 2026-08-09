#ifndef RENDERTEXTDTO_H
#define RENDERTEXTDTO_H
#include <QString>
struct RenderTextDTO
{
    double x;
    double y;
    QString text;
    RenderTextDTO(double x, double y, const QString &text)
        : x(x)
        , y(y)
        , text(text)
    {}
};
#endif // RENDERTEXTDTO_H
