#ifndef RENDERCIRCLEDTO_H
#define RENDERCIRCLEDTO_H
#include <QString>
struct ToolCircleDTO
{
    double x;
    double y;
    double radius;
    QString colorToken;
    ToolCircleDTO(double x, double y, double radius, const QString &colorToken = QString())
        : x(x)
        , y(y)
        , radius(radius)
        , colorToken(colorToken)
    {}
};
#endif // RENDERCIRCLEDTO_H