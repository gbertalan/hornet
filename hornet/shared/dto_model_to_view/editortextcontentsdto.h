#pragma once
#include <QString>
#include <QVector>

struct EditorTextContentsDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;
    QString fileType;

    explicit EditorTextContentsDTO(QVector<QString> textLinesToDisplay,
                                   int noOfAllLines,
                                   int noOfCharsOfLongestLine,
                                   QString fileType)
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
        , fileType(fileType)
    {}
};
