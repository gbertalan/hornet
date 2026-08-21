#include "editormodel.h"
#include <algorithm>

EditorModel::EditorModel()
    : m_noOfVisibleLines(0)
    , m_topLineIndex(0)
    , m_noOfCharsOfLongestLine(0)
    , m_cursorX(0)
    , m_cursorY(0)
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

int EditorModel::getCursorX() const
{
    return m_cursorX;
}

int EditorModel::getCursorY() const
{
    return m_cursorY;
}

void EditorModel::setSelection(int anchorX, int anchorY, int extentX, int extentY, bool hasSelection)
{
    m_selectionAnchorX = anchorX;
    m_selectionAnchorY = anchorY;
    m_selectionExtentX = extentX;
    m_selectionExtentY = extentY;
    m_hasSelection = hasSelection;
}

int EditorModel::getSelectionAnchorX() const
{
    return m_selectionAnchorX;
}

int EditorModel::getSelectionAnchorY() const
{
    return m_selectionAnchorY;
}

int EditorModel::getSelectionExtentX() const
{
    return m_selectionExtentX;
}

int EditorModel::getSelectionExtentY() const
{
    return m_selectionExtentY;
}

bool EditorModel::hasSelection() const
{
    return m_hasSelection;
}

void EditorModel::setCursor(int cursorX, int cursorY)
{
    m_cursorX = cursorX;
    m_cursorY = cursorY;
}

int EditorModel::getLineHeight() const
{
    return m_lineHeight;
}
void EditorModel::setLineHeight(int lineHeight)
{
    m_lineHeight = lineHeight;
}
float EditorModel::getFontScale() const
{
    return m_fontScale;
}
void EditorModel::setFontScale(float fontScale)
{
    m_fontScale = fontScale;
}

bool EditorModel::isTerminal() const
{
    return m_isTerminal;
}
void EditorModel::setIsTerminal(bool isTerminal)
{
    m_isTerminal = isTerminal;
}
