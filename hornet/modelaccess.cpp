#include "modelaccess.h"

ModelAccess::ModelAccess(Model *model) : m_model(model) {}

int ModelAccess::getValue() const {
    return m_model->getValue();
}

void ModelAccess::setValue(int value) {
    m_model->setValue(value);
}
