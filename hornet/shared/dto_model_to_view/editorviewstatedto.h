#pragma once
#include <QString>
#include <QVector>

struct EditorViewStateDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;
    QString fileType;
    QVector<QString> terminalPrompts;
    explicit EditorViewStateDTO(QVector<QString> textLinesToDisplay,
                                int noOfAllLines,
                                int noOfCharsOfLongestLine,
                                QString fileType,
                                QVector<QString> terminalPrompts)
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
        , fileType(fileType)
        , terminalPrompts(terminalPrompts)
    {}
};
