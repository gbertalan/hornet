#ifndef RENDERSCRIPTPARSER_H
#define RENDERSCRIPTPARSER_H
#include <QString>
#include <QVector>
#include "shared/dto_model_to_view/renderlinedto.h"
#include <vector>
class RenderScriptParser
{
public:
    static std::vector<RenderLineDTO> parse(const QVector<QString> &bodyLines);
};
#endif // RENDERSCRIPTPARSER_H
