#pragma once
#include "model.h"

class ModelAccess {
public:
    explicit ModelAccess(Model *model);
    int getValue() const;
    void setValue(int value);
private:
    Model *m_model;
};
