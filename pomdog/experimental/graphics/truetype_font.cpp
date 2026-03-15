// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/math/point2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
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
namespace {

class TrueTypeFontImpl final : public TrueTypeFont {
private:
    std::vector<u8> ttfBinary_ = {};
    stbtt_fontinfo fontInfo_ = {};

public:
    [[nodiscard]] std::unique_ptr<Error>
    loadFromBinary(std::vector<u8>&& fontData);

    [[nodiscard]] std::optional<FontGlyph>
    rasterizeGlyph(
        char32_t codePoint,
        float pixelHeight,
        int textureWidth,
        const std::function<void(int width, int height, Point2D& point, u8*& output)>& callback) override;
};

std::unique_ptr<Error>
TrueTypeFontImpl::loadFromBinary(std::vector<u8>&& fontData)
{
    if (fontData.empty()) {
        return errors::make("font data is empty");
    }

    ttfBinary_ = std::move(fontData);

    const auto offset = stbtt_GetFontOffsetForIndex(ttfBinary_.data(), 0);
    if (!stbtt_InitFont(&fontInfo_, ttfBinary_.data(), offset)) {
        ttfBinary_.clear();
        return errors::make("failed to initialize truetype font from binary data");
    }

    return nullptr;
}

std::optional<FontGlyph>
TrueTypeFontImpl::rasterizeGlyph(
    char32_t codePoint,
    float pixelHeight,
    int textureWidth,
    const std::function<void(int width, int height, Point2D& point, u8*& output)>& callback)
{
    if (ttfBinary_.empty()) {
        return std::nullopt;
    }

    const auto& f = fontInfo_;
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

    u8* pixels = nullptr;
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

    POMDOG_ASSERT(static_cast<int>(scale * advance) <= static_cast<int>(std::numeric_limits<i16>::max()));

    FontGlyph glyph;
    glyph.subrect.x = point.x;
    glyph.subrect.y = point.y;
    glyph.subrect.width = glyphWidth;
    glyph.subrect.height = glyphHeight;
    glyph.texturePage = 0;
    glyph.xAdvance = static_cast<i16>(scale * advance);
    glyph.xOffset = static_cast<i16>(x0);
    glyph.yOffset = static_cast<i16>(y0);
    glyph.character = codePoint;
    return glyph;
}

} // namespace

TrueTypeFont::~TrueTypeFont() = default;

std::tuple<std::shared_ptr<TrueTypeFont>, std::unique_ptr<Error>>
loadTrueTypeFont(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath)
{
    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "cannot open font file, " + filePath));
    }

    auto [info, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(statErr), "cannot stat font file, " + filePath));
    }

    std::vector<u8> fontData(static_cast<std::size_t>(info.size));
    auto [bytesRead, readErr] = file->read(std::span<u8>(fontData));
    if (readErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(readErr), "cannot read font file, " + filePath));
    }

    auto font = std::make_shared<TrueTypeFontImpl>();
    if (auto err = font->loadFromBinary(std::move(fontData)); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to load font, " + filePath));
    }

    return std::make_tuple(std::move(font), nullptr);
}

} // namespace pomdog
