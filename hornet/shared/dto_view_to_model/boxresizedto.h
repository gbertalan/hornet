#ifndef BOXRESIZEDTO_H
#define BOXRESIZEDTO_H
#include <QPoint>
#include "shared/dto_view_to_model/boxresizeedge.h"
struct BoxResizeDTO
{
    int boxId;
    BoxResizeEdge edge;
    QPoint cellDelta; // in grid cells, not pixels
    BoxResizeDTO(int boxId, BoxResizeEdge edge, QPoint cellDelta)
        : boxId(boxId)
        , edge(edge)
        , cellDelta(cellDelta)
    {}
};
#endif // BOXRESIZEDTO_H
