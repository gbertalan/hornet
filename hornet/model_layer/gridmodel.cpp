#include "gridmodel.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

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

const std::vector<BoxModel> &GridModel::getBoxes() const
{
    return m_boxes;
}

BoxModel &GridModel::getBox(int id)
{
    for (BoxModel &box : m_boxes)
        if (box.getId() == id)
            return box;
    throw std::runtime_error("BoxModel not found for id: " + std::to_string(id));
}

int GridModel::addBox(int posX,
                      int posY,
                      int width,
                      int height,
                      const QString &headerText,
                      const QVector<QString> &bodyLines,
                      bool isFileBacked,
                      const QString &originFilePath)
{
    const int id = m_nextBoxId++;
    m_boxes.emplace_back(id,
                         posX,
                         posY,
                         width,
                         height,
                         headerText,
                         bodyLines,
                         isFileBacked,
                         originFilePath);
    return id;
}

void GridModel::removeBox(int id)
{
    m_boxes.erase(std::remove_if(m_boxes.begin(),
                                 m_boxes.end(),
                                 [id](const BoxModel &box) { return box.getId() == id; }),
                  m_boxes.end());
}

int GridModel::getSelectedBoxId() const
{
    return m_selectedBoxId;
}

void GridModel::setSelectedBoxId(int boxId)
{
    m_selectedBoxId = boxId;
}
