#ifndef TOOLTRUSTPROMPTDTO_H
#define TOOLTRUSTPROMPTDTO_H
#include <QString>
#include <QStringList>
struct ToolTrustPromptDTO
{
    int boxId;
    QStringList untrustedCommands;
    ToolTrustPromptDTO(int boxId, const QStringList &untrustedCommands)
        : boxId(boxId)
        , untrustedCommands(untrustedCommands)
    {}
};
#endif // TOOLTRUSTPROMPTDTO_H