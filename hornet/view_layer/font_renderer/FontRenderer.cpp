#include "FontRenderer.h"
#include <algorithm>
#include <qdebug.h>
#include <vector>

FontRenderer::FontRenderer(FontAtlas &atlas)
    : m_atlas(atlas)
{}

const QImage &FontRenderer::getOrCreateTintedTile(const FontAtlas::GlyphInfo &glyphInfo,
                                                  const QImage &atlasPage,
                                                  const QColor &color)
{
    const uint64_t codepointPart = static_cast<uint64_t>(
        (glyphInfo.atlasRect.left() << 16) ^ glyphInfo.atlasRect.top() ^ glyphInfo.pageIdx);
    const uint64_t colorPart = static_cast<uint64_t>(color.rgb());
    const uint64_t key = (codepointPart << 32) | colorPart;

    // Every lookup - whether a fresh insert or a cache hit - bumps this
    // tile's "last used" marker to the current counter value. This is what
    // makes eviction below true least-recently-used rather than merely
    // oldest-inserted: a tile that's reused every frame keeps getting its
    // marker refreshed, so it never looks "old" just because it happened
    // to be cached a while ago.
    ++m_tileUseCounter;

    auto it = m_tintedTileCache.find(key);
    if (it != m_tintedTileCache.end()) {
        it->second.lastUsed = m_tileUseCounter;
        return it->second.image;
    }

    const int srcW = glyphInfo.atlasRect.width();
    const int srcH = glyphInfo.atlasRect.height();
    const int srcX = glyphInfo.atlasRect.left();
    const int srcY = glyphInfo.atlasRect.top();
    const int cr = color.red();
    const int cg = color.green();
    const int cb = color.blue();

    QImage tile(srcW, srcH, QImage::Format_ARGB32_Premultiplied);
    for (int row = 0; row < srcH; ++row) {
        const uchar *srcLine = atlasPage.constScanLine(srcY + row) + srcX;
        uint *dstLine = reinterpret_cast<uint *>(tile.scanLine(row));
        for (int c = 0; c < srcW; ++c) {
            const int a = srcLine[c];
            const int pr = cr * a / 255;
            const int pg2 = cg * a / 255;
            const int pb = cb * a / 255;
            dstLine[c] = (static_cast<uint>(a) << 24) | (static_cast<uint>(pr) << 16)
                         | (static_cast<uint>(pg2) << 8) | static_cast<uint>(pb);
        }
    }

    if (m_tintedTileCache.size() >= m_maxCachedTiles)
        evictLeastRecentlyUsedTiles();

    auto insertedIt = m_tintedTileCache.emplace(key, CachedTile{std::move(tile), m_tileUseCounter});
    return insertedIt.first->second.image;
}

void FontRenderer::evictLeastRecentlyUsedTiles()
{
    // Simple bounded-cache strategy: this is NOT a linked-list LRU. Each
    // tile just remembers the counter value it was last used at (see
    // getOrCreateTintedTile). When the cache is full, we sort all entries
    // by that value and drop the oldest 20% - cheap to implement, and an
    // O(n log n) sort over a few thousand entries is negligible compared
    // to the per-character tinting work it's preventing.
    //
    // Thrashing check: if the cache filled back up (triggering another
    // eviction) after fewer drawText calls than the cache's own capacity,
    // that's a strong sign more distinct (character, color) pairs are
    // needed on screen at once than the cache can hold - see backlog note
    // on thrashing above ~5000 concurrently-visible distinct pairs.
#ifdef QT_DEBUG
    const uint64_t callsSinceLastEviction = m_tileUseCounter - m_tileUseCounterAtLastEviction;
    if (callsSinceLastEviction < m_maxCachedTiles)
        qDebug() << "FontRenderer: tinted-tile cache likely thrashing -"
                 << "only" << callsSinceLastEviction << "draw calls since last eviction"
                 << "(cache capacity:" << m_maxCachedTiles << ")";
#endif

    std::vector<std::pair<uint64_t, uint64_t>> entriesByLastUsed; // (lastUsed, key)
    entriesByLastUsed.reserve(m_tintedTileCache.size());
    for (const auto &pair : m_tintedTileCache)
        entriesByLastUsed.emplace_back(pair.second.lastUsed, pair.first);

    std::sort(entriesByLastUsed.begin(), entriesByLastUsed.end());

    const size_t evictCount = m_tintedTileCache.size() / 5; // 20%
    for (size_t i = 0; i < evictCount; ++i)
        m_tintedTileCache.erase(entriesByLastUsed[i].second);

    m_tileUseCounterAtLastEviction = m_tileUseCounter;
}

void FontRenderer::drawText(
    QPainter &painter, float x, float y, const QString &text, const QColor &color, float scale)
{
    if (m_atlas.cellWidth() == 0 || text.isEmpty())
        return;
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    const float cellW = static_cast<float>(m_atlas.cellWidth()) * scale;
    const float ascender = static_cast<float>(m_atlas.getAscenderPx()) * scale;
    const float baselineY = y + ascender;
    const QVector<uint> codepoints = text.toUcs4();
    int col = 0;
    for (uint cp : codepoints) {
        const FontAtlas::GlyphInfo *g = m_atlas.ensureGlyph(static_cast<uint32_t>(cp));
        const float cellOriginX = x + static_cast<float>(col) * cellW;
        if (g && !g->atlasRect.isEmpty()) {
            const float gx = cellOriginX + static_cast<float>(g->bearingX) * scale;
            const float gy = baselineY - static_cast<float>(g->bearingY) * scale;
            const float gw = static_cast<float>(g->atlasRect.width()) * scale;
            const float gh = static_cast<float>(g->atlasRect.height()) * scale;
            const QImage &page = m_atlas.page(g->pageIdx);
            const QImage &tile = getOrCreateTintedTile(*g, page, color);
            painter.drawImage(QRectF(gx, gy, gw, gh),
                              tile,
                              QRectF(0, 0, tile.width(), tile.height()));
        }
        ++col;
    }
}

float FontRenderer::charWidth(float scale) const
{
    return m_atlas.textWidth(1, scale);
}
