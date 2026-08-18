#ifndef TOOLDROPDOWNACTIVATEDDTO_H
#define TOOLDROPDOWNACTIVATEDDTO_H
#include <QString>
#include <QStringList>
struct ToolDropdownActivatedDTO
{
    int boxId;
    QString fieldName;
    QStringList options;
    QString currentValue;
    ToolDropdownActivatedDTO(int boxId,
                             const QString &fieldName,
                             const QStringList &options,
                             const QString &currentValue)
        : boxId(boxId)
        , fieldName(fieldName)
        , options(options)
        , currentValue(currentValue)
    {}
};
#endif // TOOLDROPDOWNACTIVATEDDTO_H