#include "windowservice.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "model_layer/windowmodel.h"

#include <qdebug.h>

WindowService::WindowService(IModelAccessReadWrite& modelAccess)
    : m_modelAccess(modelAccess) {}

void WindowService::saveWindowState(int x, int y, int width, int height, bool isFullscreen) {
    m_modelAccess.getWindowModel().setX(x);
    m_modelAccess.getWindowModel().setY(y);
    m_modelAccess.getWindowModel().setWidth(width);
    m_modelAccess.getWindowModel().setHeight(height);
    m_modelAccess.getWindowModel().setFullscreen(isFullscreen);
}
