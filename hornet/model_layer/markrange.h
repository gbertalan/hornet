#pragma once
#include <QVector>
struct MarkRange
{
    int startLine = 0;
    int endLine = 0;    // inclusive
    QString colorToken; // empty = default mark color
    MarkRange() = default;
    MarkRange(int startLine, int endLine, const QString &colorToken = QString())
        : startLine(startLine)
        , endLine(endLine)
        , colorToken(colorToken)
    {}
};
