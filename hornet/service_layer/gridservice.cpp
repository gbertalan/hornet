#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/boxresizedto.h"
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
        const int headerHeightUnits = 3;
        const int visibleLineCount = std::max(0, box.getHeight() - headerHeightUnits);
        const QVector<QString> allBodyLines = box.getBodyLines();
        const int totalLineCount = static_cast<int>(allBodyLines.size());
        const int scrollStart = std::min(box.getBodyScrollOffset(), totalLineCount);
        const int scrollEnd = std::min(scrollStart + visibleLineCount, totalLineCount);
        const QVector<QString> visibleBodyLines = allBodyLines.mid(scrollStart,
                                                                   scrollEnd - scrollStart);

        boxViewDTOs.push_back(BoxViewDTO{box.getId(),
                                         box.getPosX(),
                                         box.getPosY(),
                                         box.getWidth(),
                                         box.getHeight(),
                                         box.getHeaderText(),
                                         visibleBodyLines,
                                         totalLineCount,
                                         scrollStart,
                                         box.getCursorX(),
                                         box.getCursorY()});
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
    return BoxContentDTO{box.getHeaderText(),
                         box.getBodyLines(),
                         box.getContentType(),
                         box.getCursorX(),
                         box.getCursorY()};
}

void GridService::updateBoxContent(int boxId,
                                   const QVector<QString> &bodyLines,
                                   int cursorX,
                                   int cursorY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setBodyLines(bodyLines);
    box.setCursorPos(cursorX, cursorY);
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

void GridService::setBoxScrollOffset(int boxId, int scrollOffset)
{
    m_modelAccess.getGridModel().getBox(boxId).setBodyScrollOffset(scrollOffset);
}

void GridService::resizeBox(const BoxResizeDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    BoxModel &box = gridModel.getBox(dto.boxId);
    const int dx = dto.cellDelta.x();
    const int dy = dto.cellDelta.y();

    switch (dto.edge) {
    case BoxResizeEdge::Left:
        box.setPosX(box.getPosX() + dx);
        box.setWidth(box.getWidth() - dx);
        break;
    case BoxResizeEdge::Right:
        box.setWidth(box.getWidth() + dx);
        break;
    case BoxResizeEdge::Top:
        box.setPosY(box.getPosY() + dy);
        box.setHeight(box.getHeight() - dy);
        break;
    case BoxResizeEdge::Bottom:
        box.setHeight(box.getHeight() + dy);
        break;
    case BoxResizeEdge::TopLeft:
        box.setPosX(box.getPosX() + dx);
        box.setWidth(box.getWidth() - dx);
        box.setPosY(box.getPosY() + dy);
        box.setHeight(box.getHeight() - dy);
        break;
    case BoxResizeEdge::TopRight:
        box.setWidth(box.getWidth() + dx);
        box.setPosY(box.getPosY() + dy);
        box.setHeight(box.getHeight() - dy);
        break;
    case BoxResizeEdge::BottomLeft:
        box.setPosX(box.getPosX() + dx);
        box.setWidth(box.getWidth() - dx);
        box.setHeight(box.getHeight() + dy);
        break;
    case BoxResizeEdge::BottomRight:
        box.setWidth(box.getWidth() + dx);
        box.setHeight(box.getHeight() + dy);
        break;
    case BoxResizeEdge::None:
        break;
    }
}

void GridService::setBoxPosition(int boxId, int posX, int posY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setPosX(posX);
    box.setPosY(posY);
}

void GridService::setBoxSize(int boxId, int width, int height)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setWidth(width);
    box.setHeight(height);
}

void GridService::setCursorPosition(int boxId, int cursorX, int cursorY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setCursorPos(cursorX, cursorY);
}

void GridService::setZoomLevel(int zoomLevel)
{
    m_modelAccess.getGridModel().setZoomLevel(zoomLevel);
}

void GridService::setGridOffset(int offsetX, int offsetY)
{
    m_modelAccess.getGridModel().setOffset(QPoint(offsetX, offsetY));
}

void GridService::removeBox(int boxId)
{
    m_modelAccess.getGridModel().getBox(boxId); // throws if missing, same guard as other setters
    m_modelAccess.getGridModel().removeBox(boxId);
}
