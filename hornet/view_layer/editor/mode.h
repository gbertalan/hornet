#ifndef MODE_H
#define MODE_H

class Mode
{
protected:
public:
    Mode();
    virtual void enterPressed();
    virtual ~Mode() = default;
};

#endif // MODE_H
