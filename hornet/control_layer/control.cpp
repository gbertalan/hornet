#include "control.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/numbermodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/numberservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/numberdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"
#include <stdexcept>

Control::Control(IModelAccessRead& modelAccess, NumberService& service, WindowService& windowService, View& view)
    : m_modelAccess(modelAccess), m_service(service), m_windowService(windowService), m_view(view) {}

void Control::init() {
    NumberDTO dto{m_modelAccess.getNumberModel().getValue()};
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

void Control::onWindowStateChanged(const WindowDTO& dto) {
    m_windowService.storeWindowState(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDto &dto)
{
    // m_windowService.storeWindowState(dto);
    qDebug() << "Control::onEditorStateChanged called:" << dto.noOfVisibleLines;
    qDebug() << "Control::onEditorStateChanged called:" << dto.topLineIndex;
}

void Control::onDebugRequested() {
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const {
    const WindowModel& windowModel = m_modelAccess.getWindowModel();
    qDebug() << "===" << " MODEL STATE"<< debugPrintCounter << "===";
    qDebug() << "WindowModel:";
    qDebug() << "    " << "x:" << windowModel.getX() << "y:" << windowModel.getY() << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight() << "fullscreen:" << windowModel.isFullscreen();
    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
