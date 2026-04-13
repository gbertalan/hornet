#pragma once

#include <QByteArray>
#include <QImage>
#include <QRect>
#include <QString>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief The GlyphAtlas class The atlas is a large image that holds the images of characters we have encountered.
 */
class GlyphAtlas
{
public:
    static constexpr int BakeSize  = 32;    // character render height in pixels
    static constexpr int AtlasSize = 2048;  // atlas page side length in pixels
    static constexpr int GlyphPad  = 1;     // one pixel gap between glyphs on the atlas

    struct GlyphInfo
    {
        int   pageIdx;    // atlas page index
        QRect atlasRect;  // the cell of the glyph (empty for zero-size glyphs)
        // Bearings to position the character in the middle of its cell:
        int   bearingX;   // distance from left edge
        int   bearingY;   // distance from top edge
    };

    GlyphAtlas();
    ~GlyphAtlas();

    // Preventing multiple instances of GLyphAtlas:
    GlyphAtlas(const GlyphAtlas&) = delete;
    GlyphAtlas& operator=(const GlyphAtlas&) = delete;

    // Two ways to load a font:

    /**
     * @brief addFont Load from file
     * @param path
     * @param faceIndex
     * @return
     */
    bool addFont(const QString& path, int faceIndex = 0);

    /**
     * @brief addFontFromData Load from memory
     * @param data
     * @param faceIndex
     * @return
     */
    bool addFontFromData(const QByteArray& data, int faceIndex = 0);


    // Width of one cell at scale 1.0
    int cellWidth()  const {
        return m_cellWidth;
    }

    // Height of one cell
    int cellHeight() const {
        return BakeSize;
    }

    /**
     * @brief ascenderPx How many pixels above the baseline the tallest letter reaches.
     * @return
     */
    int getAscenderPx() const {
        return m_ascenderPx;
    }

    float textWidth(int numChars, float scale) const
    {
        return static_cast<float>(numChars) * static_cast<float>(m_cellWidth) * scale;
    }

    /**
     * @brief ensureGlyph It guarantees that the given character is rasterized and in the atlas.
     * If it's already there, it just returns the cached GlyphInfo.
     * If it's not, it rasterizes it, packs it into the atlas, caches it, and then returns the info.
     * @param codepoint A number identifying a character,
     * e.g. 65 for A' or 0x4E2D for a Chinese character
     * @return nullptr only if absolutely no font in the chain can render the character, not even as U+FFFD
     */
    const GlyphInfo* ensureGlyph(uint32_t codepoint);

    // Number of atlas pages currently allocated.
    int pageCount() const { return static_cast<int>(m_atlases.size()); }

    // The raw Grayscale8 image for page idx.
    const QImage& page(int idx) const { return m_atlases[idx]; }

    /**
     * @return The number of unique characters rasterized so far
     */
    int cachedGlyphCount() const { return static_cast<int>(m_cache.size()); }

private:
    // One horizontal shelf inside an atlas page (simple shelf packer).
    struct Shelf { int x, y, height; };

    /**
     * @brief allocRect Finds a free spot on an atlas page big enough to fit a w×h pixel glyph,
     * and returns which page it's on and the exact rectangle.
     * If no existing page has room, it creates a new one.
     * @param w
     * @param h
     * @return (page number, allocated rectangle)
     */
    std::pair<int, QRect> allocRect(int w, int h);

    /**
     * @brief copyGlyphPixelDataToAtlas Copies the raw pixel data from a FreeType bitmap into
     * the atlas page.
     * @param pageIdx
     * @param rect What allocRect returned
     * @param bmp
     */
    void copyGlyphPixelDataToAtlas(int pageIdx, QRect rect, const FT_Bitmap& bmp);

    // Derive m_cellWidth and m_ascenderPx from the primary face's metrics.
    void recomputeCellMetrics();

    FT_Library               m_ft      = nullptr;
    std::vector<FT_Face>     m_faces;
    std::vector<QByteArray>  m_fontData;  // keeps the raw font file bytes alive in memory

    std::unordered_map<uint32_t, GlyphInfo> m_cache;

    std::vector<QImage>              m_atlases;
    std::vector<std::vector<Shelf>>  m_shelves; // one list of shelves per page (each page has its own independent set of shelves)

    int m_cellWidth  = 0;
    int m_ascenderPx = 0;
};
