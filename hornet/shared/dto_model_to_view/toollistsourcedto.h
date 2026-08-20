#ifndef TOOLLISTSOURCEDTO_H
#define TOOLLISTSOURCEDTO_H
#include <QString>

struct ToolListSourceDTO
{
    QString name;
    QString command;
    int intervalMs;

    ToolListSourceDTO(const QString &name, const QString &command, int intervalMs)
        : name(name)
        , command(command)
        , intervalMs(intervalMs)
    {}
};
#endif // TOOLLISTSOURCEDTO_H