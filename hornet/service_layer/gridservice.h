#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <shared/dto_model_to_view/boxcontentdto.h>
#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/gridzoomdto.h>

struct GridDragDTO;
class IModelAccessReadWrite;
struct BoxDragDTO;
struct BoxResizeDTO;

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
               const QVector<QString> &bodyLines);
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

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
