#pragma once

class NumberModel;
class WindowModel;

class IModelAccessRead {
public:
    virtual ~IModelAccessRead() = default;
    virtual const NumberModel& numberModel() const = 0;
    virtual const WindowModel& windowModel() const = 0;
};
