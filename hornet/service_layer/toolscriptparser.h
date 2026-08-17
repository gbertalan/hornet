#ifndef TOOLSCRIPTPARSER_H
#define TOOLSCRIPTPARSER_H
#include <QHash>
#include <QString>
#include <QVector>
#include "shared/dto_model_to_view/toolscriptdto.h"
#include "shared/dto_model_to_view/toolsourcedto.h"
#include <vector>
class ToolScriptParser
{
public:
    static ToolScriptDTO parse(const QVector<QString> &bodyLines,
                               const QHash<QString, QString> &sourceValues,
                               const QHash<QString, QString> &fieldValues);
    static std::vector<ToolSourceDTO> parseSources(const QVector<QString> &bodyLines);
    static QString substituteValues(const QString &text, const QHash<QString, QString> &values);
};
#endif // TOOLSCRIPTPARSER_H
