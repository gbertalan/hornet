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
    m_gridService.addBox(2,
                         2,
                         25,
                         15,
                         "my_first_box.txt",
                         {"line one: this is the first line.",
                          "line two: and this is the second line.",
                          "line three: I can write y n m and more!"});
    m_gridService.addBox(12,
                         18,
                         25,
                         15,
                         "my_second_box.terminal",
                         {"line one: this is the first line.",
                          "line two: and this is the second line.",
                          "line three: I can write y n m and more!"});
    m_gridService.addBox(22, 2, 25, 15, "my_third_box.render", {"line 10,10,20,20"});
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}

void GridControl::dispatchGridZoomChange(const GridZoomDTO &dto)
{
    m_gridService.adjustZoom(dto);
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}

void GridControl::dispatchGridDrag(const GridDragDTO &dto)
{
    m_gridService.adjustOffset(dto);
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}

void GridControl::dispatchBoxDrag(const BoxDragDTO &dto)
{
    m_gridService.moveBoxes(dto);
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}
