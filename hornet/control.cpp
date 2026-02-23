#include "control.h"
#include <QMessageBox>
#include <stdexcept>

Control::Control(View *view, ModelAccess *modelAccess, ProjectService *service, QObject *parent)
    : QObject(parent), m_view(view), m_modelAccess(modelAccess), m_service(service) {
    connect(m_view, &View::buttonClicked, this, &Control::onButtonClicked);
}

void Control::init() {
    m_view->displayValue(m_modelAccess->getValue());
}

void Control::onButtonClicked() {
    try {
        int current = m_modelAccess->getValue();
        int doubled = m_service->doubleValue(current);
        m_modelAccess->setValue(doubled);
        m_view->displayValue(doubled);
    } catch (const std::out_of_range &e) {
        QMessageBox::warning(m_view, "Error", e.what());
    }
}
