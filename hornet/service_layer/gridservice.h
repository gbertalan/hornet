#ifndef GRIDSERVICE_H
#define GRIDSERVICE_H

class IModelAccessReadWrite;

class GridService
{
public:
    explicit GridService(IModelAccessReadWrite &modelAccess);

private:
    IModelAccessReadWrite &m_modelAccess;
};

#endif // GRIDSERVICE_H
