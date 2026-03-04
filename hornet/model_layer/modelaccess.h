#pragma once

#include "imodelaccess_readwrite.h"
#include "numbermodel.h"
#include "windowmodel.h"

class ModelAccess : public IModelAccessReadWrite {
public:
    ModelAccess();

    // first const: the caller will only get read-only access to what this numberModel() returns.
    // last const: this numberModel() will not modify ModelAccess while running.
    const NumberModel& getNumberModel() const override; // read-only

    // overloaded numberModel(), allows writing access too.
    NumberModel& getNumberModel() override; // read-write

    const WindowModel& getWindowModel() const override;
    WindowModel& getWindowModel() override;

private:
    NumberModel m_numberModel;
    WindowModel m_windowModel;
};
