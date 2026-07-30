#ifndef GRIDMODEL_H
#define GRIDMODEL_H

#include <QPoint>
#include "model_layer/boxmodel.h"
#include <vector>

class GridModel
{
public:
    GridModel();

    int getZoomLevel() const;
    void setZoomLevel(int zoomLevel);

    double getGridGap() const;

    QPoint getOffset() const;
    void setOffset(QPoint offset);

    const std::vector<BoxModel> &getBoxes() const;
    BoxModel &getBox(int id);
    int addBox(int posX,
               int posY,
               int width,
               int height,
               const QString &headerText,
               const QVector<QString> &bodyLines,
               bool isFileBacked,
               const QString &originFilePath);
    void removeBox(int id);
    int getSelectedBoxId() const;
    void setSelectedBoxId(int boxId);

private:
    int m_zoomLevel = 22;
    QPoint m_offset = {0, 0};
    std::vector<BoxModel> m_boxes;
    int m_nextBoxId = 1;

    static constexpr int m_minZoom = 0;
    static constexpr int m_maxZoom = 30;
    static constexpr double m_baseGap = 15.0;
    static constexpr int m_defaultZoom = 22;
    static constexpr double m_zoomFactor = 1.1;

    int m_selectedBoxId = -1;
};
#endif // GRIDMODEL_H
