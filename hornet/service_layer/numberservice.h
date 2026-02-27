#pragma once

#include "validator.h"
#include "dto/numberdto.h"

class IModelAccessWrite;

class NumberService {
public:
    explicit NumberService(IModelAccessWrite& modelAccess);
    NumberDTO doubleNumber();

private:
    IModelAccessWrite& m_modelAccess;
    Validator m_validator;
};
