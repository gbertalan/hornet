#include "windowcontrol.h"
#include "model_layer/imodelaccess_read.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"

WindowControl::WindowControl(IModelAccessRead &modelAccess, WindowService &windowService, View &view)
    : m_modelAccess(modelAccess)
    , m_windowService(windowService)
    , m_view(view)
{}

void WindowControl::init() {}

void WindowControl::dispatchWindowStateChanged(const WindowDTO &dto)
{
    m_windowService.storeWindowState(dto);
}

void WindowControl::sendFileNameToTitlebar(const QString &fileName)
{
    m_view.updateFileName(fileName);
}
