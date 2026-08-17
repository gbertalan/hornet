#ifndef TOOLTEXTFIELDACTIVATEDDTO_H
#define TOOLTEXTFIELDACTIVATEDDTO_H
#include <QString>
struct ToolTextFieldActivatedDTO
{
    int boxId;
    QString fieldName;
    QString currentValue;
    ToolTextFieldActivatedDTO(int boxId, const QString &fieldName, const QString &currentValue)
        : boxId(boxId)
        , fieldName(fieldName)
        , currentValue(currentValue)
    {}
};
#endif // TOOLTEXTFIELDACTIVATEDDTO_H