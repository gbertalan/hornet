#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/boxdragdto.h"
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

    std::vector<BoxViewDTO> boxViewDTOs;
    boxViewDTOs.reserve(gridModel.getBoxes().size());
    for (const BoxModel &box : gridModel.getBoxes()) {
        boxViewDTOs.push_back(BoxViewDTO{box.getId(),
                                         box.getPosX(),
                                         box.getPosY(),
                                         box.getWidth(),
                                         box.getHeight(),
                                         box.getHeaderText(),
                                         box.getBodyLines()});
    }

    return GridViewStateDTO{gridModel.getZoomLevel(),
                            gridModel.getGridGap(),
                            gridModel.getOffset(),
                            boxViewDTOs,
                            gridModel.getSelectedBoxId()};
}

void GridService::adjustOffset(const GridDragDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    gridModel.setOffset(gridModel.getOffset() + dto.delta);
}

void GridService::addBox(int posX,
                         int posY,
                         int width,
                         int height,
                         const QString &headerText,
                         const QVector<QString> &bodyLines)
{
    m_modelAccess.getGridModel().addBox(posX, posY, width, height, headerText, bodyLines);
}

void GridService::moveBoxes(const BoxDragDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    const double gridGap = gridModel.getGridGap();
    const int cellDeltaX = static_cast<int>(std::round(dto.delta.x() / gridGap));
    const int cellDeltaY = static_cast<int>(std::round(dto.delta.y() / gridGap));

    for (const int boxId : dto.boxIds) {
        BoxModel &box = gridModel.getBox(boxId);
        box.setPosX(box.getPosX() + cellDeltaX);
        box.setPosY(box.getPosY() + cellDeltaY);
    }
}

BoxContentDTO GridService::retrieveBoxContent(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return BoxContentDTO{box.getHeaderText(), box.getBodyLines(), box.getContentType()};
}

void GridService::updateBoxContent(int boxId, const QVector<QString> &bodyLines)
{
    m_modelAccess.getGridModel().getBox(boxId).setBodyLines(bodyLines);
}

int GridService::findFirstBoxIdOfType(BoxContentType contentType) const
{
    for (const BoxModel &box : m_modelAccess.getGridModel().getBoxes())
        if (box.getContentType() == contentType)
            return box.getId();
    return -1;
}

void GridService::setSelectedBox(int boxId)
{
    m_modelAccess.getGridModel().setSelectedBoxId(boxId);
}
