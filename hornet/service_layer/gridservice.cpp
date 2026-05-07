#include "gridservice.h"

GridService::GridService(IModelAccessReadWrite &modelAccess)
    : m_modelAccess(modelAccess)
{}
