#pragma once
#include <QStringList>
struct FilePathListDTO
{
    QStringList filePaths;
    explicit FilePathListDTO(QStringList filePaths)
        : filePaths(std::move(filePaths))
    {}
};