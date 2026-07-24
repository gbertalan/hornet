#ifndef BOXCONTENTDTO_H
#define BOXCONTENTDTO_H
#include <QString>
#include <QVector>

struct BoxContentDTO
{
    QString headerText;
    QVector<QString> bodyLines;
};
#endif // BOXCONTENTDTO_H
