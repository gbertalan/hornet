#ifndef MIRESULTPARSER_H
#define MIRESULTPARSER_H
#include <QString>
#include <vector>

class MiResultParser
{
public:
    // Parses a raw MI result string (e.g. "^done,register-values=[{number="0",value="0x0"},...]")
    // into one display-ready row per top-level {...} tuple found. Falls back to a single row
    // containing the whole input if no tuples are found (e.g. a pure scalar result). Does not
    // recurse into tuples/lists nested inside a tuple's own value (e.g. disasm) - those render
    // as raw text within their row, not split further.
    static std::vector<QString> parseRows(const QString &resultText);
};
#endif // MIRESULTPARSER_H