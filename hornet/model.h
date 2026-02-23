#pragma once

class Model {
public:
    Model();
    void setValue(int value);
    int getValue() const;
private:
    int m_value;
};
