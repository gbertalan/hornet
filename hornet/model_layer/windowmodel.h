#ifndef WINDOWMODEL_H
#define WINDOWMODEL_H

class WindowModel
{
public:
    WindowModel();
    int getX() const;
    int getY() const;
    void setX(int x);
    void setY(int y);
    int getWidth() const;
    int getHeight() const;
    void setWidth(int width);
    void setHeight(int height);
    bool isFullscreen() const;
    void setFullscreen(bool isFullscreen);

private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    bool m_isFullscreen;
};

#endif // WINDOWMODEL_H
