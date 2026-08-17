#ifndef TOOLTEXTFIELDCOMMITDTO_H
#define TOOLTEXTFIELDCOMMITDTO_H
#include <QString>
struct ToolTextFieldCommitDTO
{
    int boxId;
    QString fieldName;
    QString value;
    ToolTextFieldCommitDTO(int boxId, const QString &fieldName, const QString &value)
        : boxId(boxId)
        , fieldName(fieldName)
        , value(value)
    {}
};
#endif // TOOLTEXTFIELDCOMMITDTO_H