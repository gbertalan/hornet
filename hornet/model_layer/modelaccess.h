#pragma once

#include "imodelaccess_write.h"
#include "numbermodel.h"

class ModelAccess : public IModelAccessWrite {
public:
    ModelAccess();
    const NumberModel& numberModel() const override;
    NumberModel& numberModel() override;

private:
    NumberModel m_numberModel;
};
