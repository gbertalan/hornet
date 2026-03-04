#include "windowservice.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "model_layer/windowmodel.h"

#include <qdebug.h>

WindowService::WindowService(IModelAccessReadWrite& modelAccess)
    : m_modelAccess(modelAccess) {}

void WindowService::saveWindowState(int x, int y, int width, int height, bool isFullscreen) {
    m_modelAccess.windowModel().setX(x);
    m_modelAccess.windowModel().setY(y);
    m_modelAccess.windowModel().setWidth(width);
    m_modelAccess.windowModel().setHeight(height);
    m_modelAccess.windowModel().setFullscreen(isFullscreen);
}
