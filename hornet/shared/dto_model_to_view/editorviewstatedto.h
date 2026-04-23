#pragma once
#include <QString>
#include <QVector>
struct EditorViewStateDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;
    QString fileType;
    explicit EditorViewStateDTO(QVector<QString> textLinesToDisplay,
                                int noOfAllLines,
                                int noOfCharsOfLongestLine,
                                QString fileType)
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
        , fileType(fileType)
    {}
};
