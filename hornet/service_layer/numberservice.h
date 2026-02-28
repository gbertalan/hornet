#pragma once

#include "validator.h"
#include "shared/dto/numberdto.h"

class IModelAccessReadWrite;

class NumberService {
public:
    explicit NumberService(IModelAccessReadWrite& modelAccess);
    NumberDTO doubleNumber();

private:
    IModelAccessReadWrite& m_modelAccess;
    Validator m_validator;
};
