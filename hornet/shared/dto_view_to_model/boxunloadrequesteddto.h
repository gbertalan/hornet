#ifndef BOXUNLOADREQUESTEDDTO_H
#define BOXUNLOADREQUESTEDDTO_H
struct BoxUnloadRequestedDTO
{
    int boxId;
    explicit BoxUnloadRequestedDTO(int boxId)
        : boxId(boxId)
    {}
};
#endif // BOXUNLOADREQUESTEDDTO_H