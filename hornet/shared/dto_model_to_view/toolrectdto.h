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
    double thicknessMultiplier;
    ToolRectDTO(double x,
                double y,
                double width,
                double height,
                const QString &colorToken = QString(),
                double thicknessMultiplier = 1.0)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , colorToken(colorToken)
        , thicknessMultiplier(thicknessMultiplier)
    {}
};
#endif // RENDERRECTDTO_H