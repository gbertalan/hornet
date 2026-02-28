#pragma once

#include "imodelaccess_readwrite.h"
#include "numbermodel.h"

class ModelAccess : public IModelAccessReadWrite {
public:
    ModelAccess();

    // first const: the caller will only get read-only access to what this numberModel() returns.
    // last const: this numberModel() will not modify ModelAccess while running.
    const NumberModel& numberModel() const override; // read-only

    // overloaded numberModel(), allows writing access too.
    NumberModel& numberModel() override; // read-write

private:
    NumberModel m_numberModel;
};
