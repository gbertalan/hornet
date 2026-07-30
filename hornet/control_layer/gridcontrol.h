#ifndef GRIDCONTROL_H
#define GRIDCONTROL_H

struct BoxResizeDTO;
struct BoxDragDTO;
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
    void dispatchGridZoomChange(const GridZoomDTO &dto);
    void dispatchGridDrag(const GridDragDTO &dto);
    void dispatchBoxDrag(const BoxDragDTO &dto);
    void refreshGridViewState();
    void dispatchBoxResize(const BoxResizeDTO &dto);

private:
    IModelAccessRead &m_modelAccess;
    GridService &m_gridService;
    View &m_view;
};

#endif // GRIDCONTROL_H
