#include "control.h"
#include "../model_layer/imodelaccess_read.h"
#include "../model_layer/numbermodel.h"
#include "../service_layer/numberservice.h"
#include "../service_layer/dto/numberdto.h"
#include "../view_layer/view.h"
#include <stdexcept>

Control::Control(IModelAccessRead& modelAccess, NumberService& service, View& view)
    : m_modelAccess(modelAccess), m_service(service), m_view(view) {}

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
