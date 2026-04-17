#include "editormodel.h"

EditorModel::EditorModel()
    : m_noOfVisibleLines(0)
    , m_topLineIndex(0)
{}

int EditorModel::getNoOfVisibleLines() const
{
    return m_noOfVisibleLines;
}

void EditorModel::setNoOfVisibleLines(int noOfVisibleLines)
{
    m_noOfVisibleLines = noOfVisibleLines;
}

int EditorModel::getTopLineIndex() const
{
    return m_topLineIndex;
}

void EditorModel::setTopLineIndex(int topLineIndex)
{
    m_topLineIndex = topLineIndex;
}
