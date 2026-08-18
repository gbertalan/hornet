#ifndef TOOLDROPDOWNDTO_H
#define TOOLDROPDOWNDTO_H
#include <QString>
#include <QStringList>
struct ToolDropdownDTO
{
    double x;
    double y;
    double width;
    double height;
    QString name;
    QStringList options;
    QString value;
    ToolDropdownDTO(double x,
                    double y,
                    double width,
                    double height,
                    const QString &name,
                    const QStringList &options,
                    const QString &value)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , name(name)
        , options(options)
        , value(value)
    {}
};
#endif // TOOLDROPDOWNDTO_H