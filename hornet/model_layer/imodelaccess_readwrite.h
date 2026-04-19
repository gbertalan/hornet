#pragma once

#include "imodelaccess_read.h"

class NumberModel;
class WindowModel;
class EditorModel;

class IModelAccessReadWrite : public IModelAccessRead
{
public:
    virtual ~IModelAccessReadWrite() = default;
    virtual NumberModel &getNumberModel() = 0;
    virtual WindowModel &getWindowModel() = 0;
    virtual EditorModel &getEditorModel() = 0;
};
