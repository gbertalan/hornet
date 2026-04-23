#pragma once

struct EditorUserInputDTO
{
    int cursorX;
    int cursorY;

    explicit EditorUserInputDTO(int cursorX, int cursorY)
        : cursorX(cursorX)
        , cursorY(cursorY)
    {}
};
