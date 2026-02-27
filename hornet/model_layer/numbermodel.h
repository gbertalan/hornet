#pragma once

class NumberModel {
public:
    NumberModel();
    int getValue() const;
    void setValue(int value);

private:
    int m_value;
};
