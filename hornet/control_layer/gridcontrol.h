#ifndef GRIDCONTROL_H
#define GRIDCONTROL_H

struct GridDragDTO;
class View;
class GridService;
class IModelAccessRead;
struct GridZoomDTO;

class GridControl
{
public:
    explicit GridControl(IModelAccessRead &modelAccess, GridService &gridService, View &view);
    void init();
    void handleGridZoomChange(const GridZoomDTO &dto);
    void handleGridDrag(const GridDragDTO &dto);

private:
    IModelAccessRead &m_modelAccess;
    GridService &m_gridService;
    View &m_view;
};

#endif // GRIDCONTROL_H
