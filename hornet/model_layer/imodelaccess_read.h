#pragma once

class GridModel;
class WindowModel;
class EditorModel;
class TerminalModel;

class IModelAccessRead
{
public:
    virtual ~IModelAccessRead() = default;
    virtual const WindowModel &getWindowModel() const = 0;
    virtual const EditorModel &getEditorModel() const = 0;
    virtual const TerminalModel &getTerminalModel() const = 0;
    virtual const GridModel &getGridModel() const = 0;
};
