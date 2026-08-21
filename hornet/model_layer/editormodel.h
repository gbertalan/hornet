#pragma once
#include <string>
#include <vector>
class EditorModel
{
public:
    EditorModel();
    int getNoOfVisibleLines() const;
    void setNoOfVisibleLines(int noOfVisibleLines);
    int getTopLineIndex() const;
    void setTopLineIndex(int topLineIndex);
    int getNoOfLines() const;
    int getNoOfCharsOfLongestLine() const;
    const std::vector<std::u32string> &getTextLines() const;
    void setTextLines(std::vector<std::u32string> textLines);
    const std::string &getFileType() const;
    void setFileType(std::string fileType);
    void setCursor(int cursorX, int cursorY);
    int getCursorX() const;
    int getCursorY() const;
    void setSelection(int anchorX, int anchorY, int extentX, int extentY, bool hasSelection);
    int getSelectionAnchorX() const;
    int getSelectionAnchorY() const;
    int getSelectionExtentX() const;
    int getSelectionExtentY() const;
    bool hasSelection() const;
    int getLineHeight() const;
    void setLineHeight(int lineHeight);
    float getFontScale() const;
    void setFontScale(float fontScale);
    bool isTerminal() const;
    void setIsTerminal(bool isTerminal);

private:
    void setNoOfCharsOfLongestLine();
    int m_noOfVisibleLines;
    int m_topLineIndex;
    std::vector<std::u32string> m_textLines; // u32string: one element = one codepoint, CJK-safe
    // Recompute this whenever m_textLines or any individual line is mutated.
    int m_noOfCharsOfLongestLine;
    std::string m_fileType; // ASCII (e.g. "cpp", "asm")
    int m_cursorX;          // column
    int m_cursorY;          // row
    int m_selectionAnchorX = 0;
    int m_selectionAnchorY = 0;
    int m_selectionExtentX = 0;
    int m_selectionExtentY = 0;
    bool m_hasSelection = false;
    int m_lineHeight = 20;
    float m_fontScale = 0.5f;
    bool m_isTerminal = true;
};
