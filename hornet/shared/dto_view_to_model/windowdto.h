#pragma once

struct WindowDTO {
    int x;
    int y;
    int width;
    int height;
    bool isFullscreen;

    explicit WindowDTO(int x, int y, int width, int height, bool isFullscreen)
        : x(x)
        , y(y)
        , width(width)
        , height(height)
        , isFullscreen(isFullscreen)
    {}
};
