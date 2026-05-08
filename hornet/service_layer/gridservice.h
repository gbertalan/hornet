#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include "shared/dto_bidirectional/gridzoomdto.h"

class IModelAccessReadWrite;

class GridService
{
public:
    explicit GridService(IModelAccessReadWrite &modelAccess);

    void adjustZoom(const GridZoomDTO &dto);

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
