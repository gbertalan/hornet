#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "renderscriptparser.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/boxresizedto.h"
#include "shared/dto_view_to_model/griddragdto.h"

#include "renderscriptparser.h"

GridService::GridService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}

// ================================================================
// SLICE: grid viewport (zoom, pan) + full view-state retrieval
// ================================================================

void GridService::adjustZoom(const GridZoomDTO &dto)
{
    GridModel &gridModel = m_modelAccess.getGridModel();
    const double oldGap = gridModel.getGridGap();
    const QPoint oldOffset = gridModel.getOffset();
    const int current = gridModel.getZoomLevel();
    const int adjusted = dto.scrollDirection == ScrollDirection::Up ? current + 1 : current - 1;
    gridModel.storeZoomLevel(adjusted);
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

        RenderScriptDTO renderScript;
        if (box.getContentType() == BoxContentType::RenderScript) {
            const QHash<QString, QString> sourceValues = m_renderSourceValues.value(box.getId());
            renderScript = RenderScriptParser::parse(allBodyLines, sourceValues);
        }

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
                                         box.getCursorY(),
                                         box.getContentType(),
                                         renderScript});
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
    gridModel.setOffset(dto.offset);
}

void GridService::storeZoomLevel(int zoomLevel)
{
    m_modelAccess.getGridModel().storeZoomLevel(zoomLevel);
}

void GridService::storeGridOffset(int offsetX, int offsetY)
{
    m_modelAccess.getGridModel().setOffset(QPoint(offsetX, offsetY));
}

// ================================================================
// SLICE: box lifecycle (create, remove, lookup)
// ================================================================

int GridService::addBox(int posX,
                        int posY,
                        int width,
                        int height,
                        const QString &headerText,
                        const QVector<QString> &bodyLines,
                        bool isFileBacked,
                        const QString &originFilePath)
{
    return m_modelAccess.getGridModel()
        .addBox(posX, posY, width, height, headerText, bodyLines, isFileBacked, originFilePath);
}

void GridService::removeBox(int boxId)
{
    m_modelAccess.getGridModel().getBox(boxId); // throws if missing, same guard as other setters
    m_modelAccess.getGridModel().removeBox(boxId);
}

int GridService::retrieveFirstBoxIdOfType(BoxContentType contentType) const
{
    for (const BoxModel &box : m_modelAccess.getGridModel().getBoxes())
        if (box.getContentType() == contentType)
            return box.getId();
    return -1;
}

// ================================================================
// SLICE: box position/size/drag/resize
// ================================================================

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

void GridService::storeBoxPosition(int boxId, int posX, int posY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setPosX(posX);
    box.setPosY(posY);
}

void GridService::storeBoxSize(int boxId, int width, int height)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setWidth(width);
    box.setHeight(height);
}

// ================================================================
// SLICE: box content, cursor, scroll, selection
// ================================================================

BoxContentDTO GridService::retrieveBoxContent(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return BoxContentDTO{box.getHeaderText(),
                         box.getBodyLines(),
                         box.getContentType(),
                         box.getCursorX(),
                         box.getCursorY()};
}

void GridService::storeBoxContent(int boxId,
                                   const QVector<QString> &bodyLines,
                                   int cursorX,
                                   int cursorY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setBodyLines(bodyLines);
    box.setCursorPos(cursorX, cursorY);
}

void GridService::storeSelectedBox(int boxId)
{
    m_modelAccess.getGridModel().storeSelectedBoxId(boxId);
}

void GridService::storeBoxScrollOffset(int boxId, int scrollOffset)
{
    m_modelAccess.getGridModel().getBox(boxId).setBodyScrollOffset(scrollOffset);
}

void GridService::storeCursorPosition(int boxId, int cursorX, int cursorY)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setCursorPos(cursorX, cursorY);
}

// ================================================================
// SLICE: hornet save
// ================================================================

GridSaveDataDTO GridService::retrieveGridSaveData() const
{
    const GridModel &gridModel = m_modelAccess.getGridModel();
    std::vector<BoxSaveDataDTO> boxes;
    boxes.reserve(gridModel.getBoxes().size());
    for (const BoxModel &box : gridModel.getBoxes()) {
        boxes.push_back(BoxSaveDataDTO{box.getId(),
                                       box.getPosX(),
                                       box.getPosY(),
                                       box.getWidth(),
                                       box.getHeight(),
                                       box.getBodyScrollOffset(),
                                       box.getCursorX(),
                                       box.getCursorY(),
                                       box.getIsFileBacked(),
                                       box.getOriginFilePath()});
    }
    return GridSaveDataDTO{gridModel.getZoomLevel(), gridModel.getOffset(), boxes};
}

std::vector<RenderSourceDTO> GridService::retrieveRenderSources(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return RenderScriptParser::parseSources(box.getBodyLines());
}

// ================================================================
// SLICE: render
// ================================================================

void GridService::storeRenderSourceValue(int boxId, const QString &sourceName, const QString &value)
{
    m_renderSourceValues[boxId][sourceName] = value;
}

int GridService::retrieveBoxCount() const
{
    return static_cast<int>(m_modelAccess.getGridModel().getBoxes().size());
}

std::vector<BoxListEntryDTO> GridService::retrieveBoxHeaderListPage(int startIndex, int count) const
{
    std::vector<BoxListEntryDTO> allEntries;
    const std::vector<BoxModel> &boxes = m_modelAccess.getGridModel().getBoxes();
    allEntries.reserve(boxes.size());
    for (const BoxModel &box : boxes)
        allEntries.push_back(BoxListEntryDTO{box.getId(), box.getHeaderText()});

    std::sort(allEntries.begin(),
              allEntries.end(),
              [](const BoxListEntryDTO &a, const BoxListEntryDTO &b) {
                  return a.headerText.localeAwareCompare(b.headerText) < 0;
              });

    const int total = static_cast<int>(allEntries.size());
    const int clampedStart = std::clamp(startIndex, 0, total);
    const int clampedEnd = std::min(clampedStart + count, total);
    return std::vector<BoxListEntryDTO>(allEntries.begin() + clampedStart,
                                        allEntries.begin() + clampedEnd);
}
