#pragma once

#include <QPoint>

struct GridDragDTO
{
    QPoint delta;
    QPoint position;

    explicit GridDragDTO(QPoint delta, QPoint position)
        : delta(delta)
        , position(position)
    {}
};
