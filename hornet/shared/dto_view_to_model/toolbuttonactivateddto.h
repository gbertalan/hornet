#ifndef TOOLBUTTONACTIVATEDDTO_H
#define TOOLBUTTONACTIVATEDDTO_H
#include <QString>
struct ToolButtonActivatedDTO
{
    int boxId;
    QString hornetCommand;
    ToolButtonActivatedDTO(int boxId, const QString &hornetCommand)
        : boxId(boxId)
        , hornetCommand(hornetCommand)
    {}
};
#endif // TOOLBUTTONACTIVATEDDTO_H