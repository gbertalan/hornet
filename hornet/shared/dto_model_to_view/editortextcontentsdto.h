#pragma once
#include <QString>
#include <QVector>

struct EditorTextContentsDto
{
    QVector<QString> textLines;

    explicit EditorTextContentsDto(QVector<QString> textLines)
        : textLines(textLines)
    {}
};
