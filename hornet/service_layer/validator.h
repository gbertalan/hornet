#pragma once

class Validator {
public:
    bool validate(int value) const;

private:
    static constexpr int MIN_VALUE = 0;
    static constexpr int MAX_VALUE = 1000000000;
};
