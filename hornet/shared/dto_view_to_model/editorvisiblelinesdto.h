#pragma once

struct EditorVisibleLinesDto
{
    int noOfVisibleLines;
    int topLineIndex;

    explicit EditorVisibleLinesDto(int noOfVisibleLines, int topLineIndex)
        : noOfVisibleLines(noOfVisibleLines)
        , topLineIndex(topLineIndex)
    {}
};
