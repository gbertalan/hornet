#ifndef RENDERRECTDTO_H
#define RENDERRECTDTO_H
#include <QString>
struct ToolRectDTO
{
    double x;
    double y;
    double width;
    double height;
    QString colorToken;
    ToolRectDTO(
        double x, double y, double width, double height, const QString &colorToken = QString())
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , colorToken(colorToken)
    {}
};
#endif // RENDERRECTDTO_H