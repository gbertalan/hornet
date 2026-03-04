#include "windowmodel.h"

WindowModel::WindowModel() {}

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
