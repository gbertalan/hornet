#pragma once

#include "GlyphAtlas.h"

#include <QPainter>

class CpuGlyphRenderer
{
public:
    explicit CpuGlyphRenderer(GlyphAtlas& atlas);

    void drawText(QPainter&      painter,
                  float          x,
                  float          y,
                  const QString& text,
                  const QColor&  color,
                  float          scale = 1.0f);
    float charWidth(float scale = 1.0f) const;

private:
    GlyphAtlas& m_atlas;
};

