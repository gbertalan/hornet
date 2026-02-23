#include "model.h"

Model::Model() : m_value(42) {}

void Model::setValue(int value) {
    m_value = value;
}

int Model::getValue() const {
    return m_value;
}
