#include "modelaccess.h"

ModelAccess::ModelAccess() {}

const NumberModel& ModelAccess::numberModel() const {
    return m_numberModel;
}

NumberModel& ModelAccess::numberModel() {
    return m_numberModel;
}

const WindowModel& ModelAccess::windowModel() const {
    return m_windowModel;
}

WindowModel& ModelAccess::windowModel() {
    return m_windowModel;
}
