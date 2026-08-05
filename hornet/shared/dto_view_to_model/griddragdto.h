#pragma once

#include <QPoint>

struct GridDragDTO
{
    QPoint offset;
    QPoint position;
    explicit GridDragDTO(QPoint offset, QPoint position)
        : offset(offset)
        , position(position)
    {}
};
