#pragma once

#include "imodelaccess_read.h"

class NumberModel;

class IModelAccessWrite : public IModelAccessRead {
public:
    virtual ~IModelAccessWrite() = default;
    virtual NumberModel& numberModel() = 0;
};
