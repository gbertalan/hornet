#include "renderscriptparser.h"

std::vector<RenderLineDTO> RenderScriptParser::parse(const QVector<QString> &bodyLines)
{
    std::vector<RenderLineDTO> result;
    for (const QString &rawLine : bodyLines) {
        const QString line = rawLine.trimmed();
        if (!line.startsWith("line "))
            continue;

        const QString argsText = line.mid(5).trimmed();
        const QStringList parts = argsText.split(' ', Qt::SkipEmptyParts);
        if (parts.size() != 4)
            continue;

        bool x1Ok = false, y1Ok = false, x2Ok = false, y2Ok = false;
        const double x1 = parts.at(0).toDouble(&x1Ok);
        const double y1 = parts.at(1).toDouble(&y1Ok);
        const double x2 = parts.at(2).toDouble(&x2Ok);
        const double y2 = parts.at(3).toDouble(&y2Ok);
        if (!x1Ok || !y1Ok || !x2Ok || !y2Ok)
            continue;

        result.push_back(RenderLineDTO(x1, y1, x2, y2));
    }
    return result;
}
