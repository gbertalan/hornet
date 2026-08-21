#include "gridservice.h"
#include "model_layer/gridmodel.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "model_layer/markrange.h"
#include "shared/dto_view_to_model/boxdragdto.h"
#include "shared/dto_view_to_model/boxresizedto.h"
#include "shared/dto_view_to_model/griddragdto.h"
#include "toolscriptparser.h"

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

        ToolScriptDTO renderScript;
        if (box.getContentType() == BoxContentType::Tool) {
            const QHash<QString, QString> sourceValues = m_toolSourceValues.value(box.getId());
            const QHash<QString, QString> fieldValues = box.getToolFieldValues();
            renderScript = ToolScriptParser::parse(allBodyLines, sourceValues, fieldValues);
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
                                         box.getSelectionAnchorX(),
                                         box.getSelectionAnchorY(),
                                         box.getSelectionExtentX(),
                                         box.getSelectionExtentY(),
                                         box.hasSelection(),
                                         box.getMarks(),
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
                         box.getCursorY(),
                         box.getSelectionAnchorX(),
                         box.getSelectionAnchorY(),
                         box.getSelectionExtentX(),
                         box.getSelectionExtentY(),
                         box.hasSelection(),
                         box.getMarks()};
}

QString GridService::retrieveBoxOriginFilePath(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return box.getOriginFilePath();
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

void GridService::storeBoxSelection(
    int boxId, int anchorX, int anchorY, int extentX, int extentY, bool hasSelection)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setSelection(anchorX, anchorY, extentX, extentY, hasSelection);
}

void GridService::storeBoxMarks(int boxId, const QVector<MarkRange> &marks)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setMarks(marks);
}

QVector<MarkRange> GridService::computeMarksAfterSubtract(const QVector<MarkRange> &marks,
                                                          MarkRange toRemove) const
{
    QVector<MarkRange> result;
    for (const MarkRange &mark : marks) {
        if (toRemove.endLine < mark.startLine || toRemove.startLine > mark.endLine) {
            result.push_back(mark);
            continue;
        }
        if (toRemove.startLine > mark.startLine)
            result.push_back(MarkRange(mark.startLine, toRemove.startLine - 1, mark.colorToken));
        if (toRemove.endLine < mark.endLine)
            result.push_back(MarkRange(toRemove.endLine + 1, mark.endLine, mark.colorToken));
    }
    return result;
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
                                       box.isFileBacked(),
                                       box.getOriginFilePath()});
    }
    return GridSaveDataDTO{gridModel.getZoomLevel(), gridModel.getOffset(), boxes};
}

std::vector<ToolSourceDTO> GridService::retrieveToolSources(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return ToolScriptParser::parseSources(box.getBodyLines());
}

BoxContentType GridService::retrieveBoxContentType(int boxId) const
{
    return m_modelAccess.getGridModel().getBox(boxId).getContentType();
}

std::vector<ToolListSourceDTO> GridService::retrieveToolListSources(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return ToolScriptParser::parseListSources(box.getBodyLines());
}

int GridService::upsertListBox(const QString &name, const QVector<QString> &rows)
{
    static const QString suffix = ".list";
    const QString headerText = name + suffix;
    GridModel &gridModel = m_modelAccess.getGridModel();

    int listBoxId = -1;
    for (const BoxModel &box : gridModel.getBoxes()) {
        if (box.getHeaderText() == headerText) {
            listBoxId = box.getId();
            break;
        }
    }
    if (listBoxId == -1)
        listBoxId = gridModel.addBox(0, 0, 30, 15, headerText, rows, false, QString());
    else
        gridModel.getBox(listBoxId).setBodyLines(rows);

    return listBoxId;
}

// ================================================================
// SLICE: render
// ================================================================

void GridService::storeToolSourceValue(int boxId, const QString &sourceName, const QString &value)
{
    m_toolSourceValues[boxId][sourceName] = value;
}

int GridService::appendToLogBox(const QString &commandText, const QString &outputText)
{
    static const QString logBoxHeaderText = "hornet.log";
    GridModel &gridModel = m_modelAccess.getGridModel();

    int logBoxId = -1;
    for (const BoxModel &box : gridModel.getBoxes()) {
        if (box.getHeaderText() == logBoxHeaderText) {
            logBoxId = box.getId();
            break;
        }
    }
    if (logBoxId == -1)
        logBoxId = gridModel
                       .addBox(0, 0, 30, 15, logBoxHeaderText, QVector<QString>{}, false, QString());

    BoxModel &logBox = gridModel.getBox(logBoxId);
    QVector<QString> bodyLines = logBox.getBodyLines();
    bodyLines.push_back("$ " + commandText);
    if (outputText.isEmpty()) {
        bodyLines.push_back("(no output)");
    } else {
        for (const QString &line : outputText.split('\n'))
            bodyLines.push_back(line);
    }
    logBox.setBodyLines(bodyLines);
    return logBoxId;
}

std::vector<QString> GridService::retrieveToolButtonCommands(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return ToolScriptParser::parseButtonCommands(box.getBodyLines());
}

// ================================================================
// SLICE: tool textfield values (persisted, unlike tool source cache above)
// ================================================================

QString GridService::retrieveToolFieldValue(int boxId, const QString &name) const
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return box.getToolFieldValue(name);
}

void GridService::storeToolFieldValue(int boxId, const QString &name, const QString &value)
{
    BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    box.setToolFieldValue(name, value);
}

QHash<QString, QString> GridService::retrieveToolFieldValues(int boxId) const
{
    const BoxModel &box = m_modelAccess.getGridModel().getBox(boxId);
    return box.getToolFieldValues();
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
        allEntries.push_back(BoxListEntryDTO{box.getId(), box.getHeaderText(), box.isFileBacked()});

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

int GridService::retrieveHighestBoxId() const
{
    int highest = 0;
    for (const BoxModel &box : m_modelAccess.getGridModel().getBoxes())
        highest = std::max(highest, box.getId());
    return highest;
}
