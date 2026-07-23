#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

#include <shared/dto_model_to_view/gridviewstatedto.h>
#include <shared/dto_view_to_model/gridzoomdto.h>

class GridDragDTO;
class IModelAccessReadWrite;
class BoxDragDTO;

class GridService
{
public:
    explicit GridService(IModelAccessReadWrite &modelAccess);
    void adjustZoom(const GridZoomDTO &dto);
    GridViewStateDTO retrieveGridViewState() const;
    void adjustOffset(const GridDragDTO &dto);
    void addBox(int posX,
                int posY,
                int width,
                int height,
                const QString &headerText,
                const QVector<QString> &bodyLines);
    void moveBoxes(const BoxDragDTO &dto);

private:
    IModelAccessReadWrite &m_modelAccess;
};
#endif // GRIDSERVICE_H
