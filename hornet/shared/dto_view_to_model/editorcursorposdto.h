#pragma once

struct EditorCursorPosDTO
{
    int cursorX;
    int cursorY;

    explicit EditorCursorPosDTO(int cursorX, int cursorY)
        : cursorX(cursorX)
        , cursorY(cursorY)
    {}
};
