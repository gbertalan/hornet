#ifndef RENDERCIRCLEDTO_H
#define RENDERCIRCLEDTO_H
#include <QString>
struct ToolCircleDTO
{
    double x;
    double y;
    double radius;
    QString colorToken;
    double thicknessMultiplier;
    ToolCircleDTO(double x,
                  double y,
                  double radius,
                  const QString &colorToken = QString(),
                  double thicknessMultiplier = 1.0)
        : x(x)
        , y(y)
        , radius(radius)
        , colorToken(colorToken)
        , thicknessMultiplier(thicknessMultiplier)
    {}
};
#endif // RENDERCIRCLEDTO_H