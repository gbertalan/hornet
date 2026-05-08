#include "gridmodel.h"
#include <algorithm>
#include <cmath>

GridModel::GridModel() {}

int GridModel::getZoomLevel() const
{
    return m_zoomLevel;
}

void GridModel::setZoomLevel(int zoomLevel)
{
    m_zoomLevel = std::clamp(zoomLevel, m_minZoom, m_maxZoom);
}

double GridModel::getGridGap() const
{
    return m_baseGap * std::pow(m_zoomFactor, m_zoomLevel - m_defaultZoom);
}

QPoint GridModel::getOffset() const
{
    return m_offset;
}

void GridModel::setOffset(QPoint offset)
{
    m_offset = offset;
}
