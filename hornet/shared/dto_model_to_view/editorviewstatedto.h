#pragma once
#include <QString>
#include <QVector>
struct EditorViewStateDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;
    QString fileType;
    int cursorX;
    int cursorY;
    explicit EditorViewStateDTO(QVector<QString> textLinesToDisplay,
                                int noOfAllLines,
                                int noOfCharsOfLongestLine,
                                QString fileType,
                                int cursorX,
                                int cursorY)
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
        , fileType(fileType)
        , cursorX(cursorX)
        , cursorY(cursorY)
    {}
};
