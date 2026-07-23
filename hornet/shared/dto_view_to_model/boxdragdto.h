#ifndef BOXDRAGDTO_H
#define BOXDRAGDTO_H
#include <QPoint>
#include <vector>

struct BoxDragDTO
{
    // boxIds is a list (not a single id) so that this same DTO can later support
    // dragging multiple selected boxes at once, applying the same delta to each.
    std::vector<int> boxIds;
    QPoint delta;

    BoxDragDTO(std::vector<int> boxIds, QPoint delta)
        : boxIds(std::move(boxIds))
        , delta(delta)
    {}
};
#endif // BOXDRAGDTO_H
