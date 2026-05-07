#pragma once

#include <QPoint>

struct GridZoomDTO
{
    int zoomLevel;
    QPoint cursorPosition;

    explicit GridZoomDTO(int zoomLevel, QPoint cursorPosition)
        : zoomLevel(zoomLevel)
        , cursorPosition(cursorPosition)
    {}
};
