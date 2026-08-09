#ifndef RENDERSOURCEDTO_H
#define RENDERSOURCEDTO_H
#include <QString>
struct RenderSourceDTO
{
    QString name;
    QString command;
    RenderSourceDTO(const QString &name, const QString &command)
        : name(name)
        , command(command)
    {}
};
#endif // RENDERSOURCEDTO_H
