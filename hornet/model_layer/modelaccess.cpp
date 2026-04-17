#include "modelaccess.h"

ModelAccess::ModelAccess() {}

const NumberModel &ModelAccess::getNumberModel() const
{
    return m_numberModel;
}

NumberModel &ModelAccess::getNumberModel()
{
    return m_numberModel;
}

const WindowModel &ModelAccess::getWindowModel() const
{
    return m_windowModel;
}

WindowModel &ModelAccess::getWindowModel()
{
    return m_windowModel;
}

const EditorModel &ModelAccess::getEditorModel() const
{
    return m_editorModel;
}

EditorModel &ModelAccess::getEditorModel()
{
    return m_editorModel;
}
