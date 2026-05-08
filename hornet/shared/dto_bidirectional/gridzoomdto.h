#pragma once

#include <QPoint>

enum class ScrollDirection { Up, Down };

struct GridZoomDTO
{
    ScrollDirection scrollDirection;
    QPoint cursorPosition;

    explicit GridZoomDTO(ScrollDirection scrollDirection, QPoint cursorPosition)
        : scrollDirection(scrollDirection)
        , cursorPosition(cursorPosition)
    {}
};
