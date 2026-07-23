#ifndef BOXVIEWDTO_H
#define BOXVIEWDTO_H
#include <QString>
#include <QVector>

struct BoxViewDTO
{
    int id;
    int posX;
    int posY;
    int width;
    int height;
    QString headerText;
    QVector<QString> bodyLines;
};
#endif // BOXVIEWDTO_H
