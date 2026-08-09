#ifndef RENDERCIRCLEDTO_H
#define RENDERCIRCLEDTO_H
struct RenderCircleDTO
{
    double x;
    double y;
    double radius;
    RenderCircleDTO(double x, double y, double radius)
        : x(x)
        , y(y)
        , radius(radius)
    {}
};
#endif // RENDERCIRCLEDTO_H
