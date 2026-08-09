#include "renderscriptparser.h"

RenderScriptDTO RenderScriptParser::parse(const QVector<QString> &bodyLines)
{
    std::vector<RenderLineDTO> lines;
    std::vector<RenderRectDTO> rects;
    std::vector<RenderCircleDTO> circles;
    std::vector<RenderTextDTO> texts;

    for (const QString &rawLine : bodyLines) {
        const QString line = rawLine.trimmed();

        if (line.startsWith("line ")) {
            const QStringList parts = line.mid(5).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() != 4)
                continue;
            bool x1Ok = false, y1Ok = false, x2Ok = false, y2Ok = false;
            const double x1 = parts.at(0).toDouble(&x1Ok);
            const double y1 = parts.at(1).toDouble(&y1Ok);
            const double x2 = parts.at(2).toDouble(&x2Ok);
            const double y2 = parts.at(3).toDouble(&y2Ok);
            if (x1Ok && y1Ok && x2Ok && y2Ok)
                lines.push_back(RenderLineDTO(x1, y1, x2, y2));

        } else if (line.startsWith("rect ")) {
            const QStringList parts = line.mid(5).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() != 4)
                continue;
            bool xOk = false, yOk = false, wOk = false, hOk = false;
            const double x = parts.at(0).toDouble(&xOk);
            const double y = parts.at(1).toDouble(&yOk);
            const double w = parts.at(2).toDouble(&wOk);
            const double h = parts.at(3).toDouble(&hOk);
            if (xOk && yOk && wOk && hOk)
                rects.push_back(RenderRectDTO(x, y, w, h));

        } else if (line.startsWith("circle ")) {
            const QStringList parts = line.mid(7).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() != 3)
                continue;
            bool xOk = false, yOk = false, rOk = false;
            const double x = parts.at(0).toDouble(&xOk);
            const double y = parts.at(1).toDouble(&yOk);
            const double r = parts.at(2).toDouble(&rOk);
            if (xOk && yOk && rOk)
                circles.push_back(RenderCircleDTO(x, y, r));

        } else if (line.startsWith("text ")) {
            const QString argsText = line.mid(5).trimmed();
            const int firstSpace = argsText.indexOf(' ');
            if (firstSpace == -1)
                continue;
            const int secondSpace = argsText.indexOf(' ', firstSpace + 1);
            if (secondSpace == -1)
                continue;
            bool xOk = false, yOk = false;
            const double x = argsText.left(firstSpace).toDouble(&xOk);
            const double y = argsText.mid(firstSpace + 1, secondSpace - firstSpace - 1)
                                 .toDouble(&yOk);
            if (!xOk || !yOk)
                continue;
            const QString text = argsText.mid(secondSpace + 1);
            if (!text.isEmpty())
                texts.push_back(RenderTextDTO(x, y, text));
        }
    }

    return RenderScriptDTO(lines, rects, circles, texts);
}
