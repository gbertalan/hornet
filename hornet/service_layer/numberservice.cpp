#include "numberservice.h"
#include "../model_layer/imodelaccess_write.h"
#include "../model_layer/numbermodel.h"
#include <stdexcept>

NumberService::NumberService(IModelAccessWrite& modelAccess)
    : m_modelAccess(modelAccess) {}

NumberDTO NumberService::doubleNumber() {
    int current = m_modelAccess.numberModel().getValue();
    int result = current * 2;
    if (!m_validator.validate(result)) {
        throw std::out_of_range("Value out of range");
    }
    m_modelAccess.numberModel().setValue(result);
    return NumberDTO{result};
}
