#include "gridcontrol.h"
#include "model_layer/imodelaccess_read.h"
#include "service_layer/gridservice.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/griddragdto.h"
#include "view_layer/view.h"

#include <qdebug.h>

GridControl::GridControl(IModelAccessRead &modelAccess, GridService &gridService, View &view)
    : m_modelAccess(modelAccess)
    , m_gridService(gridService)
    , m_view(view)
{}

void GridControl::init()
{
    m_gridService.addBox(3, 3, 20, 15, ".terminal", {}, false, QString());
    sendViewStateToGrid();
}

void GridControl::dispatchGridZoomChange(const GridZoomDTO &dto)
{
    m_gridService.adjustZoom(dto);
    sendViewStateToGrid();
}

void GridControl::dispatchGridDrag(const GridDragDTO &dto)
{
    m_gridService.adjustOffset(dto);
    sendViewStateToGrid();
}

void GridControl::dispatchBoxDrag(const BoxDragDTO &dto)
{
    m_gridService.moveBoxes(dto);
    sendViewStateToGrid();
}

void GridControl::sendViewStateToGrid()
{
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}

void GridControl::dispatchBoxResize(const BoxResizeDTO &dto)
{
    m_gridService.resizeBox(dto);
    sendViewStateToGrid();
}
