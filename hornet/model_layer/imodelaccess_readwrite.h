#pragma once

#include "imodelaccess_read.h"

class NumberModel;

class IModelAccessReadWrite : public IModelAccessRead {
public:
    virtual ~IModelAccessReadWrite() = default;
    virtual NumberModel& numberModel() = 0;
};
