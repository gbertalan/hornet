#pragma once
#include <QString>
#include <QVector>

/**
 * @brief The EditorViewStateDTO class - Holds data to display the text in the editor correctly
 */
struct EditorViewStateDTO
{
    QVector<QString> textLinesToDisplay;
    int noOfAllLines;           // to set Editor height for vertical scrolling
    int noOfCharsOfLongestLine; // to set Editor width for horizontal scrolling
    QString fileType; // file extension. Can be used for syntax highlighting or file-specific display (e.g. image)
    QVector<QString> terminalPrompts; // terminal's current working directory
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
