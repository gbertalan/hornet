#include "windowmodel.h"

WindowModel::WindowModel()
    : m_x(0),
    m_y(0),
    m_width(800),
    m_height(600),
    m_isFullscreen(false)
{}

int WindowModel::getX() const{
    return m_x;
}

int WindowModel::getY() const{
    return m_y;
}

void WindowModel::setX(int x){
    m_x = x;
}

void WindowModel::setY(int y){
    m_y = y;
}

int WindowModel::getWidth() const{
    return m_width;
}

int WindowModel::getHeight() const{
    return m_height;
}

void WindowModel::setWidth(int width){
    m_width = width;
}

void WindowModel::setHeight(int height){
    m_height = height;
}

bool WindowModel::isFullscreen() const{
    return m_isFullscreen;
}

void WindowModel::setFullscreen(bool isFullscreen){
    m_isFullscreen = isFullscreen;
}
