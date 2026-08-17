#ifndef TOOLTEXTFIELDDTO_H
#define TOOLTEXTFIELDDTO_H
#include <QString>
struct ToolTextFieldDTO
{
    double x;
    double y;
    double width;
    double height;
    QString name;
    QString value;
    ToolTextFieldDTO(
        double x, double y, double width, double height, const QString &name, const QString &value)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , name(name)
        , value(value)
    {}
};
#endif // TOOLTEXTFIELDDTO_H