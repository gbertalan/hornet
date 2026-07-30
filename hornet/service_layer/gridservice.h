#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <shared/dto_model_to_view/boxcontentdto.h>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/gridzoomdto.h>

struct GridDragDTO;
class IModelAccessReadWrite;
struct BoxDragDTO;
struct BoxResizeDTO;

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
    void adjustZoom(const GridZoomDTO &dto);
    GridViewStateDTO retrieveGridViewState() const;
    void adjustOffset(const GridDragDTO &dto);
    int addBox(int posX,
               int posY,
               int width,
               int height,
               const QString &headerText,
               const QVector<QString> &bodyLines,
               bool isFileBacked,
               const QString &originFilePath);
    void moveBoxes(const BoxDragDTO &dto);
    BoxContentDTO retrieveBoxContent(int boxId) const;
    void updateBoxContent(int boxId, const QVector<QString> &bodyLines, int cursorX, int cursorY);
    int findFirstBoxIdOfType(BoxContentType contentType) const;
    void setSelectedBox(int boxId);
    void setBoxScrollOffset(int boxId, int scrollOffset);
    void resizeBox(const BoxResizeDTO &dto);
    void setBoxPosition(int boxId, int posX, int posY);
    void setBoxSize(int boxId, int width, int height);
    void setCursorPosition(int boxId, int cursorX, int cursorY);
    void setZoomLevel(int zoomLevel);
    void setGridOffset(int offsetX, int offsetY);
    void removeBox(int boxId);
    GridSaveDataDTO retrieveGridSaveData() const;

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
