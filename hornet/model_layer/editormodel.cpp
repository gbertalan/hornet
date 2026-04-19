#include "editormodel.h"
#include <algorithm>

EditorModel::EditorModel()
    : m_noOfVisibleLines(0)
    , m_topLineIndex(0)
    , m_noOfCharsOfLongestLine(0)
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

int EditorModel::getNoOfLines() const
{
    return static_cast<int>(m_textLines.size());
}

int EditorModel::getNoOfCharsOfLongestLine() const
{
    return m_noOfCharsOfLongestLine;
}

const std::vector<std::u32string> &EditorModel::getTextLines() const
{
    return m_textLines;
}

void EditorModel::setTextLines(std::vector<std::u32string> textLines)
{
    m_textLines = std::move(textLines);
    setNoOfCharsOfLongestLine();
}

void EditorModel::setNoOfCharsOfLongestLine()
{
    m_noOfCharsOfLongestLine = static_cast<int>(
        std::max_element(m_textLines.begin(),
                         m_textLines.end(),
                         [](const std::u32string &a, const std::u32string &b) {
                             return a.size() < b.size();
                         })
            ->size());
}

const std::string &EditorModel::getFileType() const
{
    return m_fileType;
}

void EditorModel::setFileType(std::string fileType)
{
    m_fileType = std::move(fileType);
}
