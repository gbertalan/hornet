#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H
#include <shared/dto_model_to_view/boxcontentdto.h>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/gridzoomdto.h>
struct GridDragDTO;
class IModelAccessReadWrite;
struct BoxDragDTO;
struct BoxResizeDTO;

// ================================================================
// SLICE: hornet save - per-box and per-grid save-data DTOs
// (not shared/dto_* since they never cross the View boundary)
// ================================================================
struct BoxSaveDataDTO
{
    int id;
    int posX;
    int posY;
    int width;
    int height;
    int scrollOffset;
    int cursorX;
    int cursorY;
    bool isFileBacked;
    QString originFilePath;
    BoxSaveDataDTO(int id,
                   int posX,
                   int posY,
                   int width,
                   int height,
                   int scrollOffset,
                   int cursorX,
                   int cursorY,
                   bool isFileBacked,
                   const QString &originFilePath)
        : id(id)
        , posX(posX)
        , posY(posY)
        , width(width)
        , height(height)
        , scrollOffset(scrollOffset)
        , cursorX(cursorX)
        , cursorY(cursorY)
        , isFileBacked(isFileBacked)
        , originFilePath(originFilePath)
    {}
};
struct GridSaveDataDTO
{
    int zoomLevel;
    QPoint offset;
    std::vector<BoxSaveDataDTO> boxes;
    GridSaveDataDTO(int zoomLevel, QPoint offset, const std::vector<BoxSaveDataDTO> &boxes)
        : zoomLevel(zoomLevel)
        , offset(offset)
        , boxes(boxes)
    {}
};

class GridService
{
public:
    explicit GridService(IModelAccessReadWrite &modelAccess);

    // ================================================================
    // SLICE: grid viewport (zoom, pan) + full view-state retrieval
    // ================================================================
    void adjustZoom(const GridZoomDTO &dto);
    GridViewStateDTO retrieveGridViewState() const;
    void adjustOffset(const GridDragDTO &dto);
    void storeZoomLevel(int zoomLevel);
    void storeGridOffset(int offsetX, int offsetY);

    // ================================================================
    // SLICE: box lifecycle (create, remove, lookup)
    // ================================================================
    int addBox(int posX,
               int posY,
               int width,
               int height,
               const QString &headerText,
               const QVector<QString> &bodyLines,
               bool isFileBacked,
               const QString &originFilePath);
    void removeBox(int boxId);
    int retrieveFirstBoxIdOfType(BoxContentType contentType) const;

    // ================================================================
    // SLICE: box position/size/drag/resize
    // ================================================================
    void moveBoxes(const BoxDragDTO &dto);
    void resizeBox(const BoxResizeDTO &dto);
    void storeBoxPosition(int boxId, int posX, int posY);
    void storeBoxSize(int boxId, int width, int height);

    // ================================================================
    // SLICE: box content, cursor, scroll, selection
    // ================================================================
    BoxContentDTO retrieveBoxContent(int boxId) const;
    void storeBoxContent(int boxId, const QVector<QString> &bodyLines, int cursorX, int cursorY);
    void storeSelectedBox(int boxId);
    void storeBoxScrollOffset(int boxId, int scrollOffset);
    void storeCursorPosition(int boxId, int cursorX, int cursorY);

    // ================================================================
    // SLICE: hornet save
    // ================================================================
    GridSaveDataDTO retrieveGridSaveData() const;

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
