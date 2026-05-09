#include "boxmodel.h"
#include <algorithm>

BoxModel::BoxModel(int id,
                   int posX,
                   int posY,
                   int width,
                   int height,
                   const QString &headerText,
                   const QVector<QString> &bodyLines)
    : m_id(id)
    , m_posX(posX)
    , m_posY(posY)
    , m_width(std::max(width, m_minWidth))
    , m_height(std::max(height, m_minHeight))
    , m_headerText(headerText)
    , m_bodyLines(bodyLines)
{}

int BoxModel::getId() const
{
    return m_id;
}
int BoxModel::getPosX() const
{
    return m_posX;
}
int BoxModel::getPosY() const
{
    return m_posY;
}
int BoxModel::getWidth() const
{
    return m_width;
}
int BoxModel::getHeight() const
{
    return m_height;
}
QString BoxModel::getHeaderText() const
{
    return m_headerText;
}
QVector<QString> BoxModel::getBodyLines() const
{
    return m_bodyLines;
}

void BoxModel::setPosX(int posX)
{
    m_posX = posX;
}
void BoxModel::setPosY(int posY)
{
    m_posY = posY;
}
void BoxModel::setWidth(int width)
{
    m_width = std::max(width, m_minWidth);
}
void BoxModel::setHeight(int height)
{
    m_height = std::max(height, m_minHeight);
}
void BoxModel::setHeaderText(const QString &headerText)
{
    m_headerText = headerText;
}
void BoxModel::setBodyLines(const QVector<QString> &bodyLines)
{
    m_bodyLines = bodyLines;
}
