#pragma once

class ModelAccess {
public:
    explicit ModelAccess();
    int getValue() const;
    void setValue(int value);
private:
    int m_value = 1;
};
