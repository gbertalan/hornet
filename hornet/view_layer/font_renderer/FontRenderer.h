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
    struct CachedTile
    {
        QImage image;
        uint64_t lastUsed;
    };
    const QImage &getOrCreateTintedTile(const FontAtlas::GlyphInfo &glyphInfo,
                                        const QImage &atlasPage,
                                        const QColor &color);
    void evictLeastRecentlyUsedTiles();
    std::unordered_map<uint64_t, CachedTile> m_tintedTileCache;
    uint64_t m_tileUseCounter = 0;
    uint64_t m_tileUseCounterAtLastEviction = 0;
    static constexpr size_t m_maxCachedTiles = 5000;

    FontAtlas &m_atlas;
};
