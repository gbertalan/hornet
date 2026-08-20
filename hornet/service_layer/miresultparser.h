#ifndef MIRESULTPARSER_H
#define MIRESULTPARSER_H
#include <QString>
#include <QStringList>
#include <vector>

class MiResultParser
{
public:
    static std::vector<QString> parseRows(const QString &resultText,
                                          const QStringList &registerNames = QStringList());
    static QStringList parseStringArray(const QString &resultText, const QString &fieldName);
};
#endif // MIRESULTPARSER_H