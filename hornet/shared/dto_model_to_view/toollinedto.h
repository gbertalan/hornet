#ifndef RENDERLINEDTO_H
#define RENDERLINEDTO_H
#include <QString>
struct ToolLineDTO
{
    double x1;
    double y1;
    double x2;
    double y2;
    QString colorToken;
    double thicknessMultiplier;
    ToolLineDTO(double x1,
                double y1,
                double x2,
                double y2,
                const QString &colorToken = QString(),
                double thicknessMultiplier = 1.0)
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
        , colorToken(colorToken)
        , thicknessMultiplier(thicknessMultiplier)
    {}
};
#endif // RENDERLINEDTO_H