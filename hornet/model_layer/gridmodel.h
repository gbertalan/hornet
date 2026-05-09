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
               const QVector<QString> &bodyLines);
    void removeBox(int id);

private:
    int m_zoomLevel = 25;
    QPoint m_offset = {0, 0};
    std::vector<BoxModel> m_boxes;
    int m_nextBoxId = 1;

    static constexpr int m_minZoom = 0;
    static constexpr int m_maxZoom = 50;
    static constexpr double m_baseGap = 30.0;
    static constexpr int m_defaultZoom = 25;
    static constexpr double m_zoomFactor = 1.1;
};
#endif // GRIDMODEL_H
