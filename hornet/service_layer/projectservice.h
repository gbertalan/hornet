#pragma once
#include "service_layer/validator.h"
#include "model_layer/modelaccess.h"

class ProjectService {
public:
    ProjectService(ModelAccess& modelAccess);
    int doubleValue(int value) const;
private:
    Validator m_validator;
};
