#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/gridzoomdto.h>

class IModelAccessReadWrite;

class GridService
{
public:
    explicit GridService(IModelAccessReadWrite &modelAccess);

    void adjustZoom(const GridZoomDTO &dto);
    GridViewStateDTO retrieveGridViewState() const;

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
