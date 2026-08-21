#include "miresultparser.h"
#include <QRegularExpression>
#include <QStringList>

// ================================================================
// SLICE: top-level tuple extraction (brace-depth aware, does NOT
// recurse into tuples/lists nested inside a tuple's own value)
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
                ++i;
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
// SLICE: key="value" extraction within a single tuple, with
// register-name substitution for the "number" key when available
// ================================================================

static QString truncateVectorValue(const QString &value)
{
    constexpr int maxLength = 40;
    if (value.length() <= maxLength)
        return value;
    return value.left(maxLength) + "...";
}

static QString formatTupleAsRow(const QString &tuple, const QStringList &registerNames)
{
    static const QRegularExpression pairPattern(
        "([A-Za-z_][A-Za-z0-9_-]*)=\"((?:[^\"\\\\]|\\\\.)*)\"");
    QRegularExpressionMatchIterator it = pairPattern.globalMatch(tuple);

    QStringList parts;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        const QString key = match.captured(1);
        QString value = match.captured(2);
        value.replace("\\\"", "\"").replace("\\\\", "\\");

        if (key == "number" && !registerNames.isEmpty()) {
            bool ok = false;
            const int index = value.toInt(&ok);
            if (ok && index >= 0 && index < registerNames.size()
                && !registerNames.at(index).isEmpty()) {
                parts.push_back("name=" + registerNames.at(index));
                continue;
            }
        }

        if (key == "value" && value.startsWith('{'))
            value = truncateVectorValue(value);

        parts.push_back(key + "=" + value);
    }
    return parts.isEmpty() ? tuple : parts.join("  ");
}

std::vector<QString> MiResultParser::parseRows(const QString &resultText,
                                               const QStringList &registerNames)
{
    const std::vector<QString> tuples = extractTopLevelTuples(resultText);
    if (tuples.empty())
        return std::vector<QString>{resultText};

    std::vector<QString> rows;
    rows.reserve(tuples.size());
    for (const QString &tuple : tuples) {
        const QString row = formatTupleAsRow(tuple, registerNames);
        rows.push_back(row.isEmpty() ? tuple : row);
    }
    return rows;
}

// ================================================================
// SLICE: flat quoted-string-array extraction (e.g.
// register-names=["rax","rbx",...]) - preserves empty-string
// entries since index position is meaningful (register number)
// ================================================================

QStringList MiResultParser::parseStringArray(const QString &resultText, const QString &fieldName)
{
    QStringList result;
    const QString marker = fieldName + "=[";
    const int startIndex = resultText.indexOf(marker);
    if (startIndex == -1)
        return result;

    int i = startIndex + marker.length();
    int depth = 1;
    const int contentStart = i;
    bool inQuotes = false;
    for (; i < resultText.length() && depth > 0; ++i) {
        const QChar c = resultText.at(i);
        if (inQuotes) {
            if (c == '\\')
                ++i;
            else if (c == '"')
                inQuotes = false;
            continue;
        }
        if (c == '"')
            inQuotes = true;
        else if (c == '[')
            ++depth;
        else if (c == ']')
            --depth;
    }
    const QString content = resultText.mid(contentStart, i - contentStart - 1);

    static const QRegularExpression itemPattern("\"((?:[^\"\\\\]|\\\\.)*)\"|(,)");
    // Walk manually to preserve empty entries between commas (regex-only globalMatch
    // would silently skip an empty "" that has no quotes at all - MI uses "" not omission,
    // so quoted-string matching alone is sufficient; kept simple since MI always quotes).
    static const QRegularExpression quotedPattern("\"((?:[^\"\\\\]|\\\\.)*)\"");
    QRegularExpressionMatchIterator it = quotedPattern.globalMatch(content);
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        QString value = match.captured(1);
        value.replace("\\\"", "\"").replace("\\\\", "\\");
        result.push_back(value);
    }
    return result;
}