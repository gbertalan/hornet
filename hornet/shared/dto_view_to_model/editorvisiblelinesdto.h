#pragma once

/**
 * @brief The EditorVisibleLinesDTO class - Transfers how many lines the Editor can display
 * and which line to start with.
 */
struct EditorVisibleLinesDTO
{
    int noOfVisibleLines;
    int topLineIndex;

    explicit EditorVisibleLinesDTO(int noOfVisibleLines, int topLineIndex)
        : noOfVisibleLines(noOfVisibleLines)
        , topLineIndex(topLineIndex)
    {}
};
