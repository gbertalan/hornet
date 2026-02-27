#pragma once

class NumberModel;

class IModelAccessRead {
public:
    virtual ~IModelAccessRead() = default;
    virtual const NumberModel& numberModel() const = 0;
};
