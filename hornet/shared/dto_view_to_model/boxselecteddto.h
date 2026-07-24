#ifndef BOXSELECTEDDTO_H
#define BOXSELECTEDDTO_H

struct BoxSelectedDTO
{
    int boxId;

    explicit BoxSelectedDTO(int boxId)
        : boxId(boxId)
    {}
};
#endif // BOXSELECTEDDTO_H
