#ifndef RENDERLINEDTO_H
#define RENDERLINEDTO_H
struct RenderLineDTO
{
    double x1;
    double y1;
    double x2;
    double y2;
    RenderLineDTO(double x1, double y1, double x2, double y2)
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
    {}
};
#endif // RENDERLINEDTO_H
