#include "modelaccess.h"

ModelAccess::ModelAccess() {}

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
