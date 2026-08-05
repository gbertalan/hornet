#pragma once
#include <QPainter>
#include "FontAtlas.h"
#include <unordered_map>
class FontRenderer
{
public:
    explicit FontRenderer(FontAtlas &atlas);
    void drawText(QPainter &painter,
                  float x,
                  float y,
                  const QString &text,
                  const QColor &color,
                  float scale = 1.0f);
    float charWidth(float scale = 1.0f) const;

private:
    // ================================================================
    // SLICE: tinted-tile cache (avoids re-tinting the same glyph/color
    // pair every frame - tinting was previously done from scratch per
    // character, per repaint, and was the actual rendering bottleneck)
    // ================================================================
    const QImage &getOrCreateTintedTile(const FontAtlas::GlyphInfo &glyphInfo,
                                        const QImage &atlasPage,
                                        const QColor &color);
    std::unordered_map<uint64_t, QImage> m_tintedTileCache;

    FontAtlas &m_atlas;
};
