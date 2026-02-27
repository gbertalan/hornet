#pragma once
#include "validator.h"

class IModelAccessWrite;

class ProjectService {
public:
    explicit ProjectService(IModelAccessWrite& modelAccess);
private:
    Validator m_validator;
};
