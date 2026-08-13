#pragma once
#include <QStringList>

struct FileLoadRequestDTO
{
    QStringList filePaths;

    explicit FileLoadRequestDTO(QStringList filePaths)
        : filePaths(std::move(filePaths))
    {}
};
