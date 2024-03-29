// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/math/point2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fstream>
#include <limits>
#include <locale>
#include <utility>
#include <vector>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#if (__clang_major__ >= 13) && !defined(__APPLE_CC__)
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#endif
#include <stb_truetype.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class TrueTypeFont::Impl final {
public:
    std::vector<std::uint8_t> ttfBinary;
    stbtt_fontinfo fontInfo;

    void Reset();

    [[nodiscard]] std::unique_ptr<Error> LoadFont(const std::string& filePath);
};

void TrueTypeFont::Impl::Reset()
{
    if (!ttfBinary.empty()) {
        ttfBinary.clear();
    }
}

std::unique_ptr<Error>
TrueTypeFont::Impl::LoadFont(const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        return errors::make("cannot open the file, " + filePath);
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        return errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        return errors::make("the font file is too small " + filePath);
    }

    Reset();

    namespace BinaryReader = pomdog::detail::BinaryReader;
    ttfBinary = BinaryReader::readArray<std::uint8_t>(stream, byteLength);

    const auto offset = stbtt_GetFontOffsetForIndex(ttfBinary.data(), 0);
    if (!stbtt_InitFont(&fontInfo, ttfBinary.data(), offset)) {
        ttfBinary.clear();

        return errors::make("failed to initialize truetype font " + filePath);
    }

    return nullptr;
}

TrueTypeFont::TrueTypeFont()
    : impl(std::make_unique<Impl>())
{
}

TrueTypeFont::~TrueTypeFont()
{
    POMDOG_ASSERT(impl);
    impl->Reset();
}

std::unique_ptr<Error>
TrueTypeFont::load(const std::string& filePath)
{
    POMDOG_ASSERT(impl);
    return impl->LoadFont(filePath);
}

std::optional<FontGlyph>
TrueTypeFont::rasterizeGlyph(
    char32_t codePoint,
    float pixelHeight,
    int textureWidth,
    const std::function<void(int width, int height, Point2D& point, std::uint8_t*& output)>& callback)
{
    if (impl->ttfBinary.empty()) {
        // error
        return std::nullopt;
    }

    const auto& f = impl->fontInfo;

    const float scale = stbtt_ScaleForPixelHeight(&f, pixelHeight);

    const int g = stbtt_FindGlyphIndex(&f, codePoint);

    // FIXME: Use `UnicodeData.txt`-generated character table instead of std::locale and std::isspace.
    std::locale defaultLocale;
    const bool isSpace = (std::isspace(static_cast<char>(codePoint), defaultLocale) != 0) && (codePoint != '\n');

    if (g <= 0 && !isSpace) {
        // error: not found
        return std::nullopt;
    }

    int advance, lsb, x0, y0, x1, y1;
    stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
    stbtt_GetGlyphBitmapBox(&f, g, scale, scale, &x0, &y0, &x1, &y1);
    const int glyphWidth = x1 - x0;
    const int glyphHeight = y1 - y0;

    std::uint8_t* pixels = nullptr;
    Point2D point = {1, 1};

    POMDOG_ASSERT(callback);
    callback(glyphWidth, glyphHeight, point, pixels);

    POMDOG_ASSERT(pixels != nullptr);
    POMDOG_ASSERT(point.x >= 0);
    POMDOG_ASSERT(point.y >= 0);
    POMDOG_ASSERT(textureWidth > 0);

    if (!isSpace) {
        stbtt_MakeGlyphBitmap(
            &f,
            pixels + point.x + point.y * textureWidth,
            glyphWidth,
            glyphHeight,
            textureWidth,
            scale,
            scale,
            g);
    }

    POMDOG_ASSERT(static_cast<int>(scale * advance) <= static_cast<int>(std::numeric_limits<std::int16_t>::max()));

    FontGlyph glyph;
    glyph.subrect.x = point.x;
    glyph.subrect.y = point.y;
    glyph.subrect.width = glyphWidth;
    glyph.subrect.height = glyphHeight;
    glyph.texturePage = 0;
    glyph.xAdvance = static_cast<std::int16_t>(scale * advance);
    glyph.xOffset = static_cast<std::int16_t>(x0);
    glyph.yOffset = static_cast<std::int16_t>(y0);
    glyph.character = codePoint;
    return glyph;
}

//int TrueTypeFont::GetBaseline(float fontSize) const
//{
//    POMDOG_ASSERT(!impl->ttfBinary.empty());
//    POMDOG_ASSERT(fontSize > 0);
//
//    if (impl->ttfBinary.empty()) {
//        // error
//        return 0;
//    }
//
//    const float scale = stbtt_ScaleForPixelHeight(&impl->fontInfo, fontSize);
//
//    int ascent = 0;
//    stbtt_GetFontVMetrics(&impl->fontInfo, &ascent, 0, 0);
//    int baseline = ascent * scale;
//    return baseline;
//}

} // namespace pomdog
