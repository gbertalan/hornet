#ifndef TOOLTRUSTPROMPTDTO_H
#define TOOLTRUSTPROMPTDTO_H
#include <QString>
#include <QStringList>
struct ToolTrustPromptDTO
{
    int boxId;
    QStringList untrustedCommands;
    QStringList untrustedSourceCommands;
    ToolTrustPromptDTO(int boxId,
                       const QStringList &untrustedCommands,
                       const QStringList &untrustedSourceCommands)
        : boxId(boxId)
        , untrustedCommands(untrustedCommands)
        , untrustedSourceCommands(untrustedSourceCommands)
    {}
};
#endif // TOOLTRUSTPROMPTDTO_H