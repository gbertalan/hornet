#ifndef TOOLRECTDTO_H
#define TOOLRECTDTO_H
#include <QString>
struct ToolRectDTO
{
    double x;
    double y;
    double width;
    double height;
    QString colorToken;
    double thicknessMultiplier;
    bool filled;
    ToolRectDTO(double x,
                double y,
                double width,
                double height,
                const QString &colorToken = QString(),
                double thicknessMultiplier = 1.0,
                bool filled = false)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , colorToken(colorToken)
        , thicknessMultiplier(thicknessMultiplier)
        , filled(filled)
    {}
};
#endif // TOOLRECTDTO_H