#pragma once
#include <QObject>

class BoxListPageDTO;
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
    void sendFileNameToTitlebar(const QString &fileName);
    void sendBoxListPageToTitlebar(const BoxListPageDTO &dto);
    void sendCurrentBoxIdToTitlebar(int boxId);

private:
    IModelAccessRead &m_modelAccess;
    WindowService &m_windowService;
    View &m_view;
};
