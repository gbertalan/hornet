#include "FontAtlas.h"
#include <QFile>
#include <cstring>

FontAtlas::FontAtlas()
{
    FT_Init_FreeType(&m_ft); // creates FreeType instance, stores it in m_ft.
}

FontAtlas::~FontAtlas()
{
    for (FT_Face face : m_faces)
        FT_Done_Face(face);
    FT_Done_FreeType(m_ft);
}

bool FontAtlas::addFont(const QString &path, int faceIndex)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    return addFontFromData(f.readAll(), faceIndex);
}

bool FontAtlas::addFontFromData(const QByteArray &data, int faceIndex)
{
    m_fontData.push_back(data);
    const QByteArray& stored = m_fontData.back();

    FT_Face face  = nullptr;
    FT_Error err  = FT_New_Memory_Face(
        m_ft,
        reinterpret_cast<const FT_Byte*>(stored.constData()),
        static_cast<FT_Long>(stored.size()),
        faceIndex,
        &face);

    if (err != 0)
    {
        m_fontData.pop_back();
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, BakeSize);
    m_faces.push_back(face);

    if (m_faces.size() == 1)
        recomputeCellMetrics();

    return true;
}

void FontAtlas::recomputeCellMetrics()
{
    if (m_faces.empty())
        return;

    FT_Face face = m_faces.front();

    if (FT_Load_Char(face, 'M', FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING) == 0)
        m_cellWidth = static_cast<int>(face->glyph->advance.x >> 6);
    else
        m_cellWidth = static_cast<int>(face->size->metrics.max_advance >> 6);

    if (m_cellWidth <= 0)
        m_cellWidth = BakeSize / 2;

    m_ascenderPx = static_cast<int>(face->size->metrics.ascender >> 6);
}

std::pair<int, QRect> FontAtlas::allocRect(int w, int h)
{
    const int pw = w + GlyphPad;
    const int ph = h + GlyphPad;

    for (int pi = 0; pi < static_cast<int>(m_atlases.size()); ++pi)
    {
        for (Shelf& shelf : m_shelves[pi])
        {
            if (shelf.x + pw <= AtlasSize && h <= shelf.height - GlyphPad)
            {
                QRect r(shelf.x, shelf.y, w, h);
                shelf.x += pw;
                return { pi, r };
            }
        }

        // Open a new shelf on this page if there is room.
        const int nextY = m_shelves[pi].empty()
            ? 0
            : m_shelves[pi].back().y + m_shelves[pi].back().height;

        if (nextY + ph <= AtlasSize)
        {
            Shelf s{ pw, nextY, ph };
            m_shelves[pi].push_back(s);
            return { pi, QRect(0, nextY, w, h) };
        }
    }

    // All pages full - allocate a new one.
    QImage img(AtlasSize, AtlasSize, QImage::Format_Grayscale8);
    img.fill(0);
    m_atlases.push_back(std::move(img));
    m_shelves.emplace_back();

    const int pi = static_cast<int>(m_atlases.size()) - 1;
    Shelf s{ pw, 0, ph };
    m_shelves[pi].push_back(s);

    return { pi, QRect(0, 0, w, h) };
}

void FontAtlas::copyGlyphPixelDataToAtlas(int pageIdx, QRect rect, const FT_Bitmap &bmp)
{
    QImage& pg = m_atlases[pageIdx];
    for (int row = 0; row < rect.height(); ++row)
    {
        uchar*       dst = pg.scanLine(rect.top() + row) + rect.left();
        const uchar* src = bmp.buffer + row * std::abs(bmp.pitch);
        std::memcpy(dst, src, static_cast<std::size_t>(rect.width()));
    }
}

const FontAtlas::GlyphInfo *FontAtlas::ensureGlyph(uint32_t codepoint)
{
    auto it = m_cache.find(codepoint);
    if (it != m_cache.end())
        return &it->second;

    // Find the first font in the fallback chain that has this codepoint.
    FT_Face chosenFace = nullptr;
    for (FT_Face face : m_faces)
    {
        if (FT_Get_Char_Index(face, codepoint) != 0)
        {
            chosenFace = face;
            break;
        }
    }

    if (!chosenFace)
    {
        // Fall back to replacement character; guard against infinite recursion.
        if (codepoint != 0xFFFDu)
            return ensureGlyph(0xFFFDu);
        return nullptr;
    }

    FT_Set_Pixel_Sizes(chosenFace, 0, BakeSize);
    if (FT_Load_Char(chosenFace, codepoint,
                     FT_LOAD_RENDER | FT_LOAD_NO_HINTING) != 0)
        return nullptr;

    const FT_GlyphSlot slot = chosenFace->glyph;
    const FT_Bitmap&   bmp  = slot->bitmap;

    const int w = static_cast<int>(bmp.width);
    const int h = static_cast<int>(bmp.rows);

    GlyphInfo info{};
    info.bearingX = slot->bitmap_left;
    info.bearingY = slot->bitmap_top;

    if (w > 0 && h > 0)
    {
        auto [pi, rect] = allocRect(w, h);
        copyGlyphPixelDataToAtlas(pi, rect, bmp);
        info.pageIdx   = pi;
        info.atlasRect = rect;
    }

    m_cache[codepoint] = info;
    return &m_cache[codepoint];
}
