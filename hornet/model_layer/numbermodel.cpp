#include "numbermodel.h"

NumberModel::NumberModel() : m_value(1) {}

int NumberModel::getValue() const {
    return m_value;
}

void NumberModel::setValue(int value) {
    m_value = value;
}
