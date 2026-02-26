#include "numbermodel.h"

ModelAccess::ModelAccess() {}

int ModelAccess::getValue() const {
    return m_value;
}

void ModelAccess::setValue(int value) {
    m_value = value;
}
