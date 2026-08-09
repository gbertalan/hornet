#ifndef RENDERSCRIPTPARSER_H
#define RENDERSCRIPTPARSER_H
#include <QString>
#include <QVector>
#include "shared/dto_model_to_view/renderscriptdto.h"
class RenderScriptParser
{
public:
    static RenderScriptDTO parse(const QVector<QString> &bodyLines);
};
#endif // RENDERSCRIPTPARSER_H
