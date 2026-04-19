#include "control.h"
#include "model_layer/editormodel.h"
#include "model_layer/imodelaccess_read.h"
#include "model_layer/numbermodel.h"
#include "model_layer/windowmodel.h"
#include "service_layer/editorservice.h"
#include "service_layer/numberservice.h"
#include "service_layer/windowservice.h"
#include "shared/dto_view_to_model/numberdto.h"
#include "shared/dto_view_to_model/windowdto.h"
#include "view_layer/view.h"
#include <stdexcept>

Control::Control(IModelAccessRead &modelAccess,
                 NumberService &service,
                 WindowService &windowService,
                 EditorService &editorService,
                 View &view)
    : m_modelAccess(modelAccess)
    , m_service(service)
    , m_windowService(windowService)
    , m_editorService(editorService)
    , m_view(view)
{}

void Control::init()
{
    NumberDTO dto{m_modelAccess.getNumberModel().getValue()};
    m_view.displayNumber(dto);
}

void Control::onButtonClicked()
{
    try {
        NumberDTO dto = m_service.doubleNumber();
        m_view.displayNumber(dto);
    } catch (const std::out_of_range &) {
        m_view.showError("Value out of range");
    }
}

void Control::onWindowStateChanged(const WindowDTO &dto)
{
    m_windowService.storeWindowState(dto);
}

void Control::onEditorStateChanged(const EditorVisibleLinesDTO &dto)
{
    m_editorService.storeEditorState(dto);
}

void Control::sendTextToEditor()
{
    // vector lines = m_modelAccess.getEditorModel(). lines
}

void Control::onDebugRequested()
{
#ifdef QT_DEBUG
    printModel();
#endif
}

void Control::printModel() const
{
    const WindowModel &windowModel = m_modelAccess.getWindowModel();
    const EditorModel &editorModel = m_modelAccess.getEditorModel();
    qDebug() << "===" << " MODEL STATE" << debugPrintCounter << "===";
    qDebug() << "WindowModel:";
    qDebug() << "    " << "x:" << windowModel.getX() << "y:" << windowModel.getY()
             << "width:" << windowModel.getWidth() << "height:" << windowModel.getHeight()
             << "fullscreen:" << windowModel.isFullscreen();
    qDebug() << "EditorModel:";
    qDebug() << "    " << "noOfVisibleLines:" << editorModel.getNoOfVisibleLines()
             << "topLineIndex:" << editorModel.getTopLineIndex();
    qDebug() << "=== MODEL STATE END ===";
    qDebug() << "";
    debugPrintCounter++;
}
