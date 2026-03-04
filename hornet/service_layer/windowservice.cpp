#include "windowservice.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "model_layer/windowmodel.h"

#include <iostream>

WindowService::WindowService(IModelAccessReadWrite& modelAccess)
    : m_modelAccess(modelAccess) {}

void WindowService::saveWindowState(int x, int y, int width, int height, bool isFullscreen) {
    std::cerr << "WindowService::saveWindowSize " << counter << " " << width << " x " << height << std::endl;
    ++counter;
    m_modelAccess.windowModel().setX(x);
    m_modelAccess.windowModel().setY(y);
    m_modelAccess.windowModel().setWidth(width);
    m_modelAccess.windowModel().setHeight(height);
    m_modelAccess.windowModel().setFullscreen(isFullscreen);
}
