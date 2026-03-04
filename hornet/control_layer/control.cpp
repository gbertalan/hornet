#include "control.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/numbermodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/numberservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto/numberdto.h"
#include "view_layer/view.h"
#include <stdexcept>

Control::Control(IModelAccessRead& modelAccess, NumberService& service, WindowService& windowService, View& view)
    : m_modelAccess(modelAccess), m_service(service), m_windowService(windowService), m_view(view) {}

void Control::init() {
    NumberDTO dto{m_modelAccess.numberModel().getValue()};
    m_view.displayNumber(dto);
}

void Control::onButtonClicked() {
    try {
        NumberDTO dto = m_service.doubleNumber();
        m_view.displayNumber(dto);
    } catch (const std::out_of_range&) {
        m_view.showError("Value out of range");
    }
}

void Control::onWindowStateChanged(int x, int y, int width, int height, bool isFullscreen) {
    m_windowService.saveWindowState(x, y, width, height, isFullscreen);
}

void Control::onDebugRequested() {
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const {
    const WindowModel& windowModel = m_modelAccess.windowModel();
    qDebug() << "=== MODEL STATE ===";
    qDebug() << "----- WindowModel";
    qDebug() << "x:" << windowModel.getX() << "y:" << windowModel.getY() << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight() << "fullscreen:" << windowModel.isFullscreen();
    qDebug() << "-----------------";
    qDebug() << "===================";
}
