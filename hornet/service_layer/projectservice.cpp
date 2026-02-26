#include "projectservice.h"
#include <stdexcept>

ProjectService::ProjectService() {}

int ProjectService::doubleValue(int value) const {
    int result = value * 2;
    if (!m_validator.validate(result)) {
        throw std::out_of_range("Value out of range");
    }
    return result;
}
