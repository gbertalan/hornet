#pragma once

#include <QPoint>
#include "shared/dto_model_to_view/boxviewdto.h"
#include <vector>

struct GridViewStateDTO
{
    int zoomLevel;
    double gridGap;
    QPoint offset;
    std::vector<BoxViewDTO> boxes;

    explicit GridViewStateDTO(int zoomLevel,
                              double gridGap,
                              QPoint offset,
                              std::vector<BoxViewDTO> boxes)
        : zoomLevel(zoomLevel)
        , gridGap(gridGap)
        , offset(offset)
        , boxes(std::move(boxes))
    {}
};
