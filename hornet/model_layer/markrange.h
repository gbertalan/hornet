#pragma once
#include <QVector>
struct MarkRange
{
    int startLine = 0;
    int endLine = 0; // inclusive
    MarkRange() = default;
    MarkRange(int startLine, int endLine)
        : startLine(startLine)
        , endLine(endLine)
    {}
};

inline QVector<MarkRange> subtractMarkRange(const QVector<MarkRange> &marks, MarkRange toRemove)
{
    QVector<MarkRange> result;
    for (const MarkRange &mark : marks) {
        if (toRemove.endLine < mark.startLine || toRemove.startLine > mark.endLine) {
            result.push_back(mark); // no overlap
            continue;
        }
        if (toRemove.startLine > mark.startLine)
            result.push_back(MarkRange(mark.startLine, toRemove.startLine - 1));
        if (toRemove.endLine < mark.endLine)
            result.push_back(MarkRange(toRemove.endLine + 1, mark.endLine));
    }
    return result;
}