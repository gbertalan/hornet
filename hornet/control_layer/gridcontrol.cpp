#include "gridcontrol.h"
#include "model_layer/imodelaccess_read.h"
#include "service_layer/gridservice.h"

#include <qdebug.h>

GridControl::GridControl(IModelAccessRead &modelAccess, GridService &gridService, View &view)
    : m_modelAccess(modelAccess)
    , m_gridService(gridService)
    , m_view(view)
{}

void GridControl::handleGridZoomChange(const GridZoomDTO &dto)
{
    qDebug() << "GridControl: zoom";
    m_gridService.adjustZoom(dto);
}
