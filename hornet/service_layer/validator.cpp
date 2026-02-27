#include "validator.h"

bool Validator::validate(int value) const {
    return value >= MIN_VALUE && value <= MAX_VALUE;
}
