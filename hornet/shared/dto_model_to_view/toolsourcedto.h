#ifndef RENDERSOURCEDTO_H
#define RENDERSOURCEDTO_H
#include <QString>
struct ToolSourceDTO
{
    QString name;
    QString command;
    int intervalMs; // 0 = run once, never repeat; otherwise repeat at this interval once trusted
    ToolSourceDTO(const QString &name, const QString &command, int intervalMs)
        : name(name)
        , command(command)
        , intervalMs(intervalMs)
    {}
};
#endif // RENDERSOURCEDTO_H
