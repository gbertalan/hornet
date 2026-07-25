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
                         15,
                         10,
                         "my_first_box.txt",
                         {"line 1: this is the first line.",
                          "line 2: this is the second line.",
                          "line 3: this is the third line.",
                          "line 4: this is the fourth line.",
                          "line 5: this is the fifth line.",
                          "line 6: this is the sixth line.",
                          "line 7: this is the seventh line.",
                          "line 8: this is the eighth line.",
                          "line 9: this is the ninth line.",
                          "line 10: this is the tenth line."});
    m_gridService.addBox(20, 2, 15, 10, "my_second_box.terminal", {});
    m_gridService.addBox(38,
                         2,
                         15,
                         10,
                         "my_third_box.render",
                         {"line 1,1,2,2",
                          "line 2,2,3,3",
                          "line 3,3,4,4",
                          "line 4,4,5,5",
                          "line 5,5,6,6",
                          "line 6,6,7,7",
                          "line 7,7,8,8",
                          "line 8,8,9,9",
                          "line 9,9,10,10",
                          "line 10,10,11,11"});
    refreshGridViewState();
}

void GridControl::dispatchGridZoomChange(const GridZoomDTO &dto)
{
    m_gridService.adjustZoom(dto);
    refreshGridViewState();
}

void GridControl::dispatchGridDrag(const GridDragDTO &dto)
{
    m_gridService.adjustOffset(dto);
    refreshGridViewState();
}

void GridControl::dispatchBoxDrag(const BoxDragDTO &dto)
{
    m_gridService.moveBoxes(dto);
    refreshGridViewState();
}

void GridControl::refreshGridViewState()
{
    const GridViewStateDTO viewStateDTO = m_gridService.retrieveGridViewState();
    m_view.updateGridViewState(viewStateDTO);
}
