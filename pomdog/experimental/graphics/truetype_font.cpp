// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/truetype_font.hpp"
#include "pomdog/application/file_system.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/content/utility/binary_reader.hpp"
#include "pomdog/experimental/graphics/font_glyph.hpp"
#include "pomdog/experimental/graphics/sprite_font.hpp"
#include "pomdog/math/point2d.hpp"
#include "pomdog/utility/assert.hpp"

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

namespace Pomdog {

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
        return Errors::New("cannot open the file, " + filePath);
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        return Errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        return Errors::New("the font file is too small " + filePath);
    }

    Reset();

    using Pomdog::Detail::BinaryReader;
    ttfBinary = BinaryReader::ReadArray<std::uint8_t>(stream, byteLength);

    const auto offset = stbtt_GetFontOffsetForIndex(ttfBinary.data(), 0);
    if (!stbtt_InitFont(&fontInfo, ttfBinary.data(), offset)) {
        ttfBinary.clear();

        return Errors::New("failed to initialize truetype font " + filePath);
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

std::unique_ptr<Error> TrueTypeFont::Load(const std::string& filePath)
{
    POMDOG_ASSERT(impl);
    return impl->LoadFont(filePath);
}

std::optional<FontGlyph> TrueTypeFont::RasterizeGlyph(
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
    POMDOG_ASSERT(point.X >= 0);
    POMDOG_ASSERT(point.Y >= 0);
    POMDOG_ASSERT(textureWidth > 0);

    if (!isSpace) {
        stbtt_MakeGlyphBitmap(&f, pixels + point.X + point.Y * textureWidth,
            glyphWidth, glyphHeight, textureWidth, scale, scale, g);
    }

    POMDOG_ASSERT(static_cast<int>(scale * advance) <= static_cast<int>(std::numeric_limits<std::int16_t>::max()));

    FontGlyph glyph;
    glyph.Subrect.X = point.X;
    glyph.Subrect.Y = point.Y;
    glyph.Subrect.Width = glyphWidth;
    glyph.Subrect.Height = glyphHeight;
    glyph.TexturePage = 0;
    glyph.XAdvance = static_cast<std::int16_t>(scale * advance);
    glyph.XOffset = static_cast<std::int16_t>(x0);
    glyph.YOffset = static_cast<std::int16_t>(y0);
    glyph.Character = codePoint;
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

} // namespace Pomdog
