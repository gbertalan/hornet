#include "miresultparser.h"
#include <QRegularExpression>
#include <QVector>
#include <utility>
#include <vector>

// ================================================================
// SLICE: generic recursive MI value representation + parser
// (handles const strings, tuples {...}, and lists [...], nested
// arbitrarily deep - e.g. disasm's src_and_asm_line -> line_asm_insn)
// ================================================================

struct MiValue
{
    enum class Kind { Const, Tuple, List };
    Kind kind = Kind::Const;
    QString constValue;
    std::vector<std::pair<QString, MiValue>>
        fields; // Tuple: named. List: name empty unless a named result.
};

static void skipWhitespace(const QString &text, int &pos)
{
    while (pos < text.length() && text.at(pos).isSpace())
        ++pos;
}

static QString parseIdentifier(const QString &text, int &pos)
{
    const int start = pos;
    while (pos < text.length()
           && (text.at(pos).isLetterOrNumber() || text.at(pos) == '_' || text.at(pos) == '-'))
        ++pos;
    return text.mid(start, pos - start);
}

static QString parseCString(const QString &text, int &pos)
{
    QString result;
    if (pos >= text.length() || text.at(pos) != '"')
        return result;
    ++pos;
    while (pos < text.length() && text.at(pos) != '"') {
        if (text.at(pos) == '\\' && pos + 1 < text.length()) {
            const QChar next = text.at(pos + 1);
            if (next == 'n') {
                result.append('\n');
                pos += 2;
                continue;
            }
            if (next == 't') {
                result.append('\t');
                pos += 2;
                continue;
            }
            if (next == '"' || next == '\\') {
                result.append(next);
                pos += 2;
                continue;
            }
            result.append(next);
            pos += 2;
            continue;
        }
        result.append(text.at(pos));
        ++pos;
    }
    if (pos < text.length())
        ++pos;
    return result;
}

static MiValue parseValue(const QString &text, int &pos);

static std::pair<QString, MiValue> parseResultPair(const QString &text, int &pos)
{
    skipWhitespace(text, pos);
    const QString key = parseIdentifier(text, pos);
    skipWhitespace(text, pos);
    if (pos < text.length() && text.at(pos) == '=')
        ++pos;
    skipWhitespace(text, pos);
    const MiValue value = parseValue(text, pos);
    return {key, value};
}

static MiValue parseTuple(const QString &text, int &pos)
{
    MiValue tuple;
    tuple.kind = MiValue::Kind::Tuple;
    ++pos;
    skipWhitespace(text, pos);
    if (pos < text.length() && text.at(pos) == '}') {
        ++pos;
        return tuple;
    }
    while (pos < text.length()) {
        tuple.fields.push_back(parseResultPair(text, pos));
        skipWhitespace(text, pos);
        if (pos < text.length() && text.at(pos) == ',') {
            ++pos;
            skipWhitespace(text, pos);
            continue;
        }
        break;
    }
    skipWhitespace(text, pos);
    if (pos < text.length() && text.at(pos) == '}')
        ++pos;
    return tuple;
}

static MiValue parseList(const QString &text, int &pos)
{
    MiValue list;
    list.kind = MiValue::Kind::List;
    ++pos;
    skipWhitespace(text, pos);
    if (pos < text.length() && text.at(pos) == ']') {
        ++pos;
        return list;
    }
    while (pos < text.length()) {
        skipWhitespace(text, pos);
        int lookahead = pos;
        const QString maybeKey = parseIdentifier(text, lookahead);
        skipWhitespace(text, lookahead);
        if (!maybeKey.isEmpty() && lookahead < text.length() && text.at(lookahead) == '=') {
            list.fields.push_back(parseResultPair(text, pos));
        } else {
            const MiValue value = parseValue(text, pos);
            list.fields.push_back({QString(), value});
        }
        skipWhitespace(text, pos);
        if (pos < text.length() && text.at(pos) == ',') {
            ++pos;
            continue;
        }
        break;
    }
    skipWhitespace(text, pos);
    if (pos < text.length() && text.at(pos) == ']')
        ++pos;
    return list;
}

static MiValue parseValue(const QString &text, int &pos)
{
    skipWhitespace(text, pos);
    if (pos >= text.length())
        return MiValue{};
    const QChar c = text.at(pos);
    if (c == '{')
        return parseTuple(text, pos);
    if (c == '[')
        return parseList(text, pos);
    if (c == '"') {
        MiValue value;
        value.kind = MiValue::Kind::Const;
        value.constValue = parseCString(text, pos);
        return value;
    }
    MiValue value;
    value.kind = MiValue::Kind::Const;
    const int start = pos;
    while (pos < text.length() && text.at(pos) != ',' && text.at(pos) != '}' && text.at(pos) != ']')
        ++pos;
    value.constValue = text.mid(start, pos - start).trimmed();
    return value;
}

static MiValue parseTopLevel(const QString &resultText)
{
    int pos = 0;
    parseIdentifier(resultText, pos); // status word (done/error/running/exit) - discarded
    skipWhitespace(resultText, pos);
    if (pos < resultText.length() && resultText.at(pos) == ',')
        ++pos;

    MiValue root;
    root.kind = MiValue::Kind::Tuple;
    skipWhitespace(resultText, pos);
    while (pos < resultText.length()) {
        root.fields.push_back(parseResultPair(resultText, pos));
        skipWhitespace(resultText, pos);
        if (pos < resultText.length() && resultText.at(pos) == ',') {
            ++pos;
            skipWhitespace(resultText, pos);
            continue;
        }
        break;
    }
    return root;
}

// ================================================================
// SLICE: flattening a parsed MiValue tree into display-ready rows
// (register-name substitution + vector-value truncation happen here)
// ================================================================

static QString formatConstPart(const QString &key,
                               const QString &value,
                               const QStringList &registerNames)
{
    if (key == "number" && !registerNames.isEmpty()) {
        bool ok = false;
        const int index = value.toInt(&ok);
        if (ok && index >= 0 && index < registerNames.size() && !registerNames.at(index).isEmpty())
            return "name=" + registerNames.at(index);
    }
    QString displayValue = value;
    if (displayValue.startsWith('{') && displayValue.length() > 40)
        displayValue = displayValue.left(40) + "...";
    return key.isEmpty() ? displayValue : key + "=" + displayValue;
}

static void flattenMiValue(const QString &key,
                           const MiValue &value,
                           const QStringList &registerNames,
                           const QString &indent,
                           QVector<QString> &rows,
                           QStringList &inlineParts)
{
    switch (value.kind) {
    case MiValue::Kind::Const:
        inlineParts.push_back(formatConstPart(key, value.constValue, registerNames));
        break;

    case MiValue::Kind::Tuple: {
        QStringList nestedInline;
        QVector<QString> nestedRows;
        for (const auto &field : value.fields)
            flattenMiValue(field.first,
                           field.second,
                           registerNames,
                           indent + "  ",
                           nestedRows,
                           nestedInline);

        QString header = indent;
        if (!key.isEmpty())
            header += key + ": ";
        header += nestedInline.join("  ");
        if (!header.trimmed().isEmpty())
            rows.push_back(header);
        for (const QString &row : nestedRows)
            rows.push_back(row);
        break;
    }

    case MiValue::Kind::List: {
        for (const auto &item : value.fields) {
            QStringList itemInline;
            QVector<QString> itemRows;
            flattenMiValue(item.first, item.second, registerNames, indent, itemRows, itemInline);
            if (!itemInline.isEmpty())
                rows.push_back(indent + itemInline.join("  "));
            for (const QString &row : itemRows)
                rows.push_back(row);
        }
        break;
    }
    }
}

// ================================================================
// SLICE: public API
// ================================================================

std::vector<QString> MiResultParser::parseRows(const QString &resultText,
                                               const QStringList &registerNames)
{
    const MiValue topLevel = parseTopLevel(resultText);

    for (const auto &field : topLevel.fields) {
        if (field.second.kind == MiValue::Kind::List && !field.second.fields.empty()) {
            QVector<QString> rows;
            QStringList unusedInline;
            flattenMiValue(QString(), field.second, registerNames, QString(), rows, unusedInline);
            if (!rows.isEmpty())
                return std::vector<QString>(rows.begin(), rows.end());
        }
    }

    for (const auto &field : topLevel.fields) {
        if (field.second.kind == MiValue::Kind::Tuple) {
            QVector<QString> rows;
            QStringList inlineParts;
            flattenMiValue(field.first, field.second, registerNames, QString(), rows, inlineParts);
            if (!inlineParts.isEmpty())
                rows.prepend(inlineParts.join("  "));
            if (!rows.isEmpty())
                return std::vector<QString>(rows.begin(), rows.end());
        }
    }

    return std::vector<QString>{resultText};
}

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