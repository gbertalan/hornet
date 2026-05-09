#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/griddragdto.h"

GridService::GridService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}

void GridService::adjustZoom(const GridZoomDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();

    const double oldGap = gridModel.getGridGap();
    const QPoint oldOffset = gridModel.getOffset();

    const int current = gridModel.getZoomLevel();
    const int adjusted = dto.scrollDirection == ScrollDirection::Up ? current + 1 : current - 1;
    gridModel.setZoomLevel(adjusted);

    const double newGap = gridModel.getGridGap();
    const double ratio = newGap / oldGap;

    const QPoint cursor = dto.cursorPosition;
    const int newOffsetX = static_cast<int>(
        std::round(cursor.x() - (cursor.x() - oldOffset.x()) * ratio));
    const int newOffsetY = static_cast<int>(
        std::round(cursor.y() - (cursor.y() - oldOffset.y()) * ratio));

    gridModel.setOffset(QPoint(newOffsetX, newOffsetY));
}

GridViewStateDTO GridService::retrieveGridViewState() const
{
    const GridModel &gridModel = m_modelAccess.getGridModel();
    return GridViewStateDTO(gridModel.getZoomLevel(), gridModel.getGridGap(), gridModel.getOffset());
}

void GridService::adjustOffset(const GridDragDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    gridModel.setOffset(gridModel.getOffset() + dto.delta);
}
