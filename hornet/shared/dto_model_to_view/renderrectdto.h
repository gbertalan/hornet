#ifndef RENDERRECTDTO_H
#define RENDERRECTDTO_H
struct RenderRectDTO
{
    double x;
    double y;
    double width;
    double height;
    RenderRectDTO(double x, double y, double width, double height)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
    {}
};
#endif // RENDERRECTDTO_H
