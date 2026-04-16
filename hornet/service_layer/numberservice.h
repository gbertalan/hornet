#pragma once

#include "shared/dto_view_to_model/numberdto.h"
#include "validator.h"

class IModelAccessReadWrite;

class NumberService {
public:
    explicit NumberService(IModelAccessReadWrite& modelAccess);
    NumberDTO doubleNumber();

private:
    IModelAccessReadWrite& m_modelAccess;
    Validator m_validator;
};
