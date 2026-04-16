#include "windowservice.h"
#include "model_layer/imodelaccess_readwrite.h"
#include "model_layer/windowmodel.h"
#include "shared/dto_view_to_model/windowdto.h"

#include <qdebug.h>

WindowService::WindowService(IModelAccessReadWrite& modelAccess)
    : m_modelAccess(modelAccess) {}

void WindowService::storeWindowState(const WindowDTO& dto) {
    m_modelAccess.getWindowModel().setX(dto.x);
    m_modelAccess.getWindowModel().setY(dto.y);
    m_modelAccess.getWindowModel().setWidth(dto.width);
    m_modelAccess.getWindowModel().setHeight(dto.height);
    m_modelAccess.getWindowModel().setFullscreen(dto.isFullscreen);
}
