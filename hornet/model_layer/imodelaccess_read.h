#pragma once

class NumberModel;
class WindowModel;

class IModelAccessRead {
public:
    virtual ~IModelAccessRead() = default;
    virtual const NumberModel& getNumberModel() const = 0;
    virtual const WindowModel& getWindowModel() const = 0;
};
