#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"

GridService::GridService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}

void GridService::adjustZoom(const GridZoomDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    const int current = gridModel.getZoomLevel();
    const int adjusted = dto.scrollDirection == ScrollDirection::Up ? current + 1 : current - 1;
    gridModel.setZoomLevel(adjusted);
}

GridViewStateDTO GridService::retrieveGridViewState() const
{
    const GridModel &gridModel = m_modelAccess.getGridModel();
    return GridViewStateDTO(gridModel.getZoomLevel(), gridModel.getGridGap(), gridModel.getOffset());
}
