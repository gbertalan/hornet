#pragma once

#include "imodelaccess_read.h"

class WindowModel;
class EditorModel;
class GridModel;

class IModelAccessReadWrite : public IModelAccessRead
{
public:
    virtual ~IModelAccessReadWrite() = default;
    virtual WindowModel &getWindowModel() = 0;
    virtual EditorModel &getEditorModel() = 0;
    virtual GridModel &getGridModel() = 0;
};
