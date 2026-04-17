#pragma once

class NumberModel;
class WindowModel;
class EditorModel;

class IModelAccessRead {
public:
    virtual ~IModelAccessRead() = default;
    virtual const NumberModel& getNumberModel() const = 0;
    virtual const WindowModel& getWindowModel() const = 0;
    virtual const EditorModel &getEditorModel() const = 0;
};
