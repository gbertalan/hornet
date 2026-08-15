#include "toolscriptparser.h"
#include <QRegularExpression>

// ================================================================
// SLICE: $name token/text substitution helpers
// ================================================================

static bool resolveNumericToken(const QString &token,
                                const QHash<QString, QString> &sourceValues,
                                double &outValue)
{
    if (token.startsWith('$')) {
        const auto it = sourceValues.constFind(token.mid(1));
        if (it == sourceValues.constEnd()) {
            outValue = 0.0; // no value fetched yet - render as 0, per design
            return true;
        }
        bool ok = false;
        outValue = it.value().toDouble(&ok);
        if (!ok)
            outValue = 0.0;
        return true;
    }
    bool ok = false;
    outValue = token.toDouble(&ok);
    return ok;
}

static QString substituteSourceValuesInText(const QString &text,
                                            const QHash<QString, QString> &sourceValues)
{
    static const QRegularExpression pattern("\\$([A-Za-z_][A-Za-z0-9_]*)");
    QString result = text;
    QRegularExpressionMatchIterator it = pattern.globalMatch(result);

    QVector<QPair<int, int>> spans;
    QVector<QString> replacements;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        const auto found = sourceValues.constFind(match.captured(1));
        if (found != sourceValues.constEnd()) {
            spans.push_back({match.capturedStart(0), match.capturedLength(0)});
            replacements.push_back(found.value());
        }
    }
    for (int i = spans.size() - 1; i >= 0; --i)
        result.replace(spans[i].first, spans[i].second, replacements[i]);
    return result;
}

// ================================================================
// SLICE: source declaration parsing
// ================================================================

std::vector<ToolSourceDTO> ToolScriptParser::parseSources(const QVector<QString> &bodyLines)
{
    std::vector<ToolSourceDTO> sources;
    for (const QString &rawLine : bodyLines) {
        const QString line = rawLine.trimmed();
        if (!line.startsWith("source "))
            continue;

        const int colonIndex = line.indexOf(':');
        if (colonIndex == -1)
            continue;

        const QString command = line.mid(colonIndex + 1).trimmed();
        if (command.isEmpty())
            continue;

        const QStringList tokensBeforeColon = line.left(colonIndex).split(' ', Qt::SkipEmptyParts);
        // tokensBeforeColon.at(0) is always "source"

        if (tokensBeforeColon.size() == 2) {
            // source name: command  -> run once, no repeat
            const QString name = tokensBeforeColon.at(1);
            if (!name.isEmpty())
                sources.push_back(ToolSourceDTO(name, command, 0));

        } else if (tokensBeforeColon.size() == 3) {
            // source <N>ms name: command  -> repeats every N ms once trusted
            const QString intervalToken = tokensBeforeColon.at(1);
            const QString name = tokensBeforeColon.at(2);
            if (name.isEmpty() || !intervalToken.endsWith("ms"))
                continue;
            bool ok = false;
            int intervalMs = intervalToken.left(intervalToken.length() - 2).toInt(&ok);
            if (!ok)
                continue;
            constexpr int minIntervalMs = 100;
            intervalMs = std::max(intervalMs, minIntervalMs);
            sources.push_back(ToolSourceDTO(name, command, intervalMs));
        }
        // else: malformed, skip silently
    }
    return sources;
}

// ================================================================
// SLICE: primitive parsing (line, rect, circle, text)
// ================================================================

ToolScriptDTO ToolScriptParser::parse(const QVector<QString> &bodyLines,
                                      const QHash<QString, QString> &sourceValues)
{
    std::vector<ToolLineDTO> lines;
    std::vector<ToolRectDTO> rects;
    std::vector<ToolCircleDTO> circles;
    std::vector<ToolTextDTO> texts;

    const auto resolveColorArg = [](const QString &token) {
        return (token.isEmpty() || token == "-1") ? QString() : token;
    };
    const auto resolveMultiplierArg = [](const QString &token) {
        if (token.isEmpty() || token == "-1")
            return 1.0;
        bool ok = false;
        const double value = token.toDouble(&ok);
        return ok ? value : 1.0;
    };

    for (const QString &rawLine : bodyLines) {
        const QString line = rawLine.trimmed();

        if (line.startsWith("line ")) {
            const QStringList parts = line.mid(5).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 4 || parts.size() > 6)
                continue;
            double x1, y1, x2, y2;
            if (resolveNumericToken(parts.at(0), sourceValues, x1)
                && resolveNumericToken(parts.at(1), sourceValues, y1)
                && resolveNumericToken(parts.at(2), sourceValues, x2)
                && resolveNumericToken(parts.at(3), sourceValues, y2)) {
                const QString colorToken = resolveColorArg(parts.size() >= 5 ? parts.at(4)
                                                                             : QString());
                const double thickness = resolveMultiplierArg(parts.size() >= 6 ? parts.at(5)
                                                                                : QString());
                lines.push_back(ToolLineDTO(x1, y1, x2, y2, colorToken, thickness));
            }

        } else if (line.startsWith("rect ")) {
            const QStringList parts = line.mid(5).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 4 || parts.size() > 6)
                continue;
            double x, y, w, h;
            if (resolveNumericToken(parts.at(0), sourceValues, x)
                && resolveNumericToken(parts.at(1), sourceValues, y)
                && resolveNumericToken(parts.at(2), sourceValues, w)
                && resolveNumericToken(parts.at(3), sourceValues, h)) {
                const QString colorToken = resolveColorArg(parts.size() >= 5 ? parts.at(4)
                                                                             : QString());
                const double thickness = resolveMultiplierArg(parts.size() >= 6 ? parts.at(5)
                                                                                : QString());
                rects.push_back(ToolRectDTO(x, y, w, h, colorToken, thickness));
            }

        } else if (line.startsWith("circle ")) {
            const QStringList parts = line.mid(7).trimmed().split(' ', Qt::SkipEmptyParts);
            if (parts.size() < 3 || parts.size() > 5)
                continue;
            double x, y, r;
            if (resolveNumericToken(parts.at(0), sourceValues, x)
                && resolveNumericToken(parts.at(1), sourceValues, y)
                && resolveNumericToken(parts.at(2), sourceValues, r)) {
                const QString colorToken = resolveColorArg(parts.size() >= 4 ? parts.at(3)
                                                                             : QString());
                const double thickness = resolveMultiplierArg(parts.size() >= 5 ? parts.at(4)
                                                                                : QString());
                circles.push_back(ToolCircleDTO(x, y, r, colorToken, thickness));
            }

        } else if (line.startsWith("text ")) {
            const QString argsText = line.mid(5).trimmed();
            const int firstSpace = argsText.indexOf(' ');
            if (firstSpace == -1)
                continue;
            const int secondSpace = argsText.indexOf(' ', firstSpace + 1);
            if (secondSpace == -1)
                continue;
            double x, y;
            const bool xOk = resolveNumericToken(argsText.left(firstSpace), sourceValues, x);
            const bool yOk = resolveNumericToken(argsText.mid(firstSpace + 1,
                                                              secondSpace - firstSpace - 1),
                                                 sourceValues,
                                                 y);
            if (!xOk || !yOk)
                continue;

            const QString remainder = argsText.mid(secondSpace + 1).trimmed();
            if (!remainder.startsWith('"'))
                continue;
            const int closingQuoteIndex = remainder.indexOf('"', 1);
            if (closingQuoteIndex == -1)
                continue;

            QString text = remainder.mid(1, closingQuoteIndex - 1);
            text = substituteSourceValuesInText(text, sourceValues);

            const QStringList trailingParts
                = remainder.mid(closingQuoteIndex + 1).trimmed().split(' ', Qt::SkipEmptyParts);
            const QString colorToken = resolveColorArg(
                trailingParts.size() >= 1 ? trailingParts.at(0) : QString());
            const double fontSize = resolveMultiplierArg(
                trailingParts.size() >= 2 ? trailingParts.at(1) : QString());

            if (!text.isEmpty())
                texts.push_back(ToolTextDTO(x, y, text, colorToken, fontSize));
        }
    }

    return ToolScriptDTO(lines, rects, circles, texts);
}
