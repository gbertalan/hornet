#ifndef GRIDMODEL_H
#define GRIDMODEL_H

#include <QPoint>

class GridModel
{
public:
    GridModel();

    int getZoomLevel() const;
    void setZoomLevel(int zoomLevel);

    double getGridGap() const;

    QPoint getOffset() const;
    void setOffset(QPoint offset);

private:
    int m_zoomLevel = 25;
    QPoint m_offset = {0, 0};

    static constexpr int m_minZoom = 0;
    static constexpr int m_maxZoom = 50;
    static constexpr double m_baseGap = 30.0;
    static constexpr int m_defaultZoom = 25;
    static constexpr double m_zoomFactor = 1.1;
};
#endif // GRIDMODEL_H
