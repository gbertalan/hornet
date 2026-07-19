#pragma once
#include <QObject>
class IModelAccessRead;
class WindowService;
class View;
struct WindowDTO;
class WindowControl : public QObject
{
    Q_OBJECT
public:
    explicit WindowControl(IModelAccessRead &modelAccess, WindowService &windowService, View &view);
    void init();
    void dispatchWindowStateChanged(const WindowDTO &dto);

private:
    IModelAccessRead &m_modelAccess;
    WindowService &m_windowService;
    View &m_view;
};
