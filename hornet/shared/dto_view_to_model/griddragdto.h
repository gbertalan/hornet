#pragma once

#include <QPoint>

struct GridDragDTO
{
    QPoint offset;
    explicit GridDragDTO(QPoint offset)
        : offset(offset)
    {}
};
