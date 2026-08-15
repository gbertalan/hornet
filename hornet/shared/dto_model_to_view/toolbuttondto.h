#ifndef TOOLBUTTONDTO_H
#define TOOLBUTTONDTO_H
#include <QString>
struct ToolButtonDTO
{
    double x;
    double y;
    double width;
    double height;
    QString label;
    QString hornetCommand;
    QString colorToken;
    ToolButtonDTO(double x,
                  double y,
                  double width,
                  double height,
                  const QString &label,
                  const QString &hornetCommand,
                  const QString &colorToken = QString())
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , label(label)
        , hornetCommand(hornetCommand)
        , colorToken(colorToken)
    {}
};
#endif // TOOLBUTTONDTO_H