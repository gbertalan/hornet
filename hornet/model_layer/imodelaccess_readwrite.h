#pragma once

#include "imodelaccess_read.h"

class NumberModel;
class WindowModel;

class IModelAccessReadWrite : public IModelAccessRead {
public:
    virtual ~IModelAccessReadWrite() = default;
    virtual NumberModel& numberModel() = 0;
    virtual WindowModel& windowModel() = 0;
};
