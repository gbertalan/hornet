#include "control.h"
#include <stdexcept>

Control::Control(View &view, ModelAccess &modelAccess, ProjectService &projectService)
    : m_view(view), m_modelAccess(modelAccess), m_service(projectService) {
}

void Control::init() {
    m_view.displayValue(m_modelAccess.getValue());
}

void Control::onButtonClicked() {
    try {
        int current = m_modelAccess.getValue();
        int doubled = m_service.doubleValue(current);
        m_modelAccess.setValue(doubled);
        m_view.displayValue(doubled);
    } catch (const std::out_of_range &e) {
        m_view.showError("Workspace invalid");
    }
}
