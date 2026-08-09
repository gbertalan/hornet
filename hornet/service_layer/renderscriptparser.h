#ifndef RENDERSCRIPTPARSER_H
#define RENDERSCRIPTPARSER_H
#include <QHash>
#include <QString>
#include <QVector>
#include "shared/dto_model_to_view/renderscriptdto.h"
#include "shared/dto_model_to_view/rendersourcedto.h"
#include <vector>
class RenderScriptParser
{
public:
    static RenderScriptDTO parse(const QVector<QString> &bodyLines,
                                 const QHash<QString, QString> &sourceValues);
    static std::vector<RenderSourceDTO> parseSources(const QVector<QString> &bodyLines);
};
#endif // RENDERSCRIPTPARSER_H
