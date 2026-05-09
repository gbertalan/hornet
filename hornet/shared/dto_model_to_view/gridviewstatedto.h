#pragma once

#include <QPoint>
#include "model_layer/boxmodel.h"
#include <vector>

struct GridViewStateDTO
{
    int zoomLevel;
    double gridGap;
    QPoint offset;
    std::vector<BoxModel> boxes;

    explicit GridViewStateDTO(int zoomLevel,
                              double gridGap,
                              QPoint offset,
                              std::vector<BoxModel> boxes)
        : zoomLevel(zoomLevel)
        , gridGap(gridGap)
        , offset(offset)
        , boxes(std::move(boxes))
    {}
};
