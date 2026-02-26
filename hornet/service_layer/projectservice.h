#pragma once
#include "validator.h"

class ProjectService {
public:
    ProjectService();
    int doubleValue(int value) const;
private:
    Validator m_validator;
};
