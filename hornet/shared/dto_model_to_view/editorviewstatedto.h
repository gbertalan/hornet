#pragma once
#include <QString>
#include <QVector>

struct EditorViewStateDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;
    int noOfCharsOfLongestLine;
    QString fileType;
    QString terminalPrompt;
    explicit EditorViewStateDTO(QVector<QString> textLinesToDisplay,
                                int noOfAllLines,
                                int noOfCharsOfLongestLine,
                                QString fileType,
                                QString terminalPrompt = QString())
        : textLinesToDisplay(textLinesToDisplay)
        , noOfAllLines(noOfAllLines)
        , noOfCharsOfLongestLine(noOfCharsOfLongestLine)
        , fileType(fileType)
        , terminalPrompt(terminalPrompt)
    {}
};
