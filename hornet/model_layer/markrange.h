#pragma once
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