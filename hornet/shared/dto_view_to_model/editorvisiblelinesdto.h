#pragma once

struct EditorVisibleLinesDTO
{
    int noOfVisibleLines;
    int topLineIndex;

    explicit EditorVisibleLinesDTO(int noOfVisibleLines, int topLineIndex)
        : noOfVisibleLines(noOfVisibleLines)
        , topLineIndex(topLineIndex)
    {}
};
