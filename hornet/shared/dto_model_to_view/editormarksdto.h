#pragma once
#include <QVector>
#include "model_layer/markrange.h"
struct EditorMarksDTO
{
    QVector<MarkRange> marks;
    explicit EditorMarksDTO(QVector<MarkRange> marks)
        : marks(std::move(marks))
    {}
};