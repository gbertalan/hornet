#pragma once

#include <QPoint>

struct GridViewStateDTO
{
    int zoomLevel;
    double gridGap;
    QPoint offset;

    explicit GridViewStateDTO(int zoomLevel, double gridGap, QPoint offset)
        : zoomLevel(zoomLevel)
        , gridGap(gridGap)
        , offset(offset)
    {}
};
