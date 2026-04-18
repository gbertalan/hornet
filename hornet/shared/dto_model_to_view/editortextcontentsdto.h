#pragma once
#include <QString>
#include <QVector>

struct EditorTextContentsDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;

    explicit EditorTextContentsDTO(QVector<QString> textLinesToDisplay,
                                   int noOfAllLines,
                                   int noOfCharsOfLongestLine)
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
    {}
};
