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

private:
    void setNoOfCharsOfLongestLine();
    int m_noOfVisibleLines;
    int m_topLineIndex;
    std::vector<std::u32string> m_textLines; // u32string: one element = one codepoint, CJK-safe
    // Recompute this whenever m_textLines or any individual line is mutated.
    int m_noOfCharsOfLongestLine;
    std::string m_fileType; // ASCII (e.g. "cpp", "asm")
};
