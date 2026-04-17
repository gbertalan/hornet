#pragma once

class EditorModel
{
public:
    EditorModel();
    int getNoOfVisibleLines() const;
    void setNoOfVisibleLines(int noOfVisibleLines);
    int getTopLineIndex() const;
    void setTopLineIndex(int topLineIndex);

private:
    int m_noOfVisibleLines;
    int m_topLineIndex;
};
