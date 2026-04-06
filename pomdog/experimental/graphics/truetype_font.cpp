// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/math/point2d.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/scope_guard.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
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

[[nodiscard]] bool
isSpace(char32_t c) noexcept
{
    return c == U' ' || c == U'\n' || c == U'\r' || c == U'\t' || c == U'\v' || c == U'\f';
}

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
        int textureHeight,
        bool sdf,
        std::optional<i32> sdfPadding,
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
    [[maybe_unused]] int textureHeight,
    bool sdf,
    std::optional<i32> sdfPadding,
    const std::function<void(int width, int height, Point2D& point, u8*& output)>& callback)
{
    if (ttfBinary_.empty()) {
        return std::nullopt;
    }

    const auto& f = fontInfo_;
    const float scale = stbtt_ScaleForPixelHeight(&f, pixelHeight);

    const int g = stbtt_FindGlyphIndex(&f, codePoint);

    const bool isSpaceChar = (isSpace(codePoint)) && (codePoint != '\n');

    if (g <= 0 && !isSpaceChar) {
        // error: not found
        return std::nullopt;
    }

    int advance, lsb, x0, y0, x1, y1;
    stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
    stbtt_GetGlyphBitmapBox(&f, g, scale, scale, &x0, &y0, &x1, &y1);
    int glyphWidth = x1 - x0;
    int glyphHeight = y1 - y0;

    unsafe_ptr<unsigned char> bmap = nullptr;
    ScopeGuard defer([&] {
        if (bmap != nullptr) {
            stbtt_FreeSDF(bmap, nullptr);
            bmap = nullptr;
        }
    });

    if (!isSpaceChar && sdf) {
        const int padding = sdfPadding.value_or(4);
        constexpr unsigned char onedgeValue = 128;
        constexpr float pixelDistScale = 48.0f;

        int w = 0;
        int h = 0;
        int xoffset = 0;
        int yoffset = 0;

        bmap = stbtt_GetGlyphSDF(
            &f,
            scale,
            g,
            padding,
            onedgeValue,
            pixelDistScale,
            &w,
            &h,
            &xoffset,
            &yoffset);

        if (bmap != nullptr) {
            x0 = xoffset;
            y0 = yoffset;
            glyphWidth = w;
            glyphHeight = h;
        }
    }

    unsafe_ptr<u8> pixels = nullptr;
    Point2D point = {1, 1};

    POMDOG_ASSERT(callback);
    callback(glyphWidth, glyphHeight, point, pixels);

    POMDOG_ASSERT(pixels != nullptr);
    POMDOG_ASSERT(point.x >= 0);
    POMDOG_ASSERT(point.y >= 0);
    POMDOG_ASSERT(textureWidth > 0);

    if (!isSpaceChar) {
        if (sdf) {
            if (bmap != nullptr) {
                const auto offset = point.x + point.y * textureWidth;

                for (int y = 0; y < glyphHeight; y++) {
                    for (int x = 0; x < glyphWidth; x++) {
                        const auto dest = offset + x + y * textureWidth;
                        POMDOG_ASSERT(dest < (textureWidth * textureHeight));
                        pixels[dest] = bmap[x + y * glyphWidth];
                    }
                }
            }
        }
        else {
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
    }

    POMDOG_ASSERT(static_cast<i32>(scale * advance) <= static_cast<i32>(std::numeric_limits<i16>::max()));

    FontGlyph glyph = {};
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
