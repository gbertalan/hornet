#pragma once

#include "FontAtlas.h"

#include <QPainter>

class FontRenderer
{
public:
    explicit FontRenderer(FontAtlas &atlas);

    void drawText(QPainter&      painter,
                  float          x,
                  float          y,
                  const QString& text,
                  const QColor&  color,
                  float          scale = 1.0f);
    float charWidth(float scale = 1.0f) const;
    int cellHeight() const;

private:
    FontAtlas &m_atlas;
};

