#include "miresultparser.h"
#include <QRegularExpression>
#include <QStringList>

// ================================================================
// SLICE: top-level tuple extraction (brace-depth aware so a nested
// tuple/list inside a value doesn't cause an early split - does NOT
// recurse into nested tuples, see header note)
// ================================================================

static std::vector<QString> extractTopLevelTuples(const QString &text)
{
    std::vector<QString> tuples;
    int depth = 0;
    int start = -1;
    bool inQuotes = false;

    for (int i = 0; i < text.length(); ++i) {
        const QChar c = text.at(i);

        if (inQuotes) {
            if (c == '\\')
                ++i; // skip escaped char
            else if (c == '"')
                inQuotes = false;
            continue;
        }

        if (c == '"') {
            inQuotes = true;
        } else if (c == '{') {
            if (depth == 0)
                start = i;
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0 && start != -1) {
                tuples.push_back(text.mid(start, i - start + 1));
                start = -1;
            }
        }
    }
    return tuples;
}

// ================================================================
// SLICE: key="value" extraction within a single tuple (top-level
// pairs only - a nested {...}/[...] as a value is captured as its
// raw text, not recursed into)
// ================================================================

static QString formatTupleAsRow(const QString &tuple)
{
    static const QRegularExpression pairPattern(
        "([A-Za-z_][A-Za-z0-9_-]*)=\"((?:[^\"\\\\]|\\\\.)*)\"");
    QRegularExpressionMatchIterator it = pairPattern.globalMatch(tuple);

    QStringList parts;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        QString value = match.captured(2);
        value.replace("\\\"", "\"").replace("\\\\", "\\");
        parts.push_back(match.captured(1) + "=" + value);
    }
    return parts.join("  ");
}

std::vector<QString> MiResultParser::parseRows(const QString &resultText)
{
    const std::vector<QString> tuples = extractTopLevelTuples(resultText);
    if (tuples.empty())
        return std::vector<QString>{resultText};

    std::vector<QString> rows;
    rows.reserve(tuples.size());
    for (const QString &tuple : tuples) {
        const QString row = formatTupleAsRow(tuple);
        rows.push_back(row.isEmpty() ? tuple : row);
    }
    return rows;
}