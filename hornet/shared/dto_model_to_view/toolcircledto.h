#ifndef TOOLCIRCLEDTO_H
#define TOOLCIRCLEDTO_H
#include <QString>
struct ToolCircleDTO
{
    double x;
    double y;
    double radius;
    QString colorToken;
    double thicknessMultiplier;
    bool filled;
    ToolCircleDTO(double x,
                  double y,
                  double radius,
                  const QString &colorToken = QString(),
                  double thicknessMultiplier = 1.0,
                  bool filled = false)
        : x(x)
        , y(y)
        , radius(radius)
        , colorToken(colorToken)
        , thicknessMultiplier(thicknessMultiplier)
        , filled(filled)
    {}
};
#endif // TOOLCIRCLEDTO_H