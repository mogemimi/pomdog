// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/math/color.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

[[nodiscard]] bool
isSpace(char32_t c) noexcept
{
    return (c == U' ') || (c == U'\t');
}

} // namespace

class SpriteFont::Impl final {
public:
    static constexpr int TextureWidth = 2048;
    static constexpr int TextureHeight = 2048;

    std::unordered_map<char32_t, FontGlyph> spriteFontMap;

    char32_t defaultCharacter = U' ';
    f32 lineSpacing = 0.0f;
    f32 spacing = 0.0f;
    f32 fontSize = 0.0f;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::vector<std::shared_ptr<gpu::Texture2D>>&& textures,
        const std::vector<FontGlyph>& glyphs,
        f32 spacing,
        f32 lineSpacing);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        f32 fontSize,
        f32 lineSpacing);

    template <typename Func>
    void forEach(const std::string& text, Func func);

    [[nodiscard]] Vector2
    measureString(const std::string& text);

    void draw(
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void prepareFonts(const std::string& text);

private:
    std::vector<std::shared_ptr<gpu::Texture2D>> textures;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice;
    std::shared_ptr<TrueTypeFont> font;
    std::vector<std::uint8_t> pixelData;
    Point2D currentPoint = {0, 0};
    int bottomY = 0;
};

[[nodiscard]] std::unique_ptr<Error>
SpriteFont::Impl::initialize(
    std::vector<std::shared_ptr<gpu::Texture2D>>&& texturesIn,
    const std::vector<FontGlyph>& glyphsIn,
    f32 spacingIn,
    f32 lineSpacingIn)
{
    lineSpacing = lineSpacingIn;
    spacing = spacingIn;
    fontSize = 0.0f;
    textures = std::move(texturesIn);

    for (auto& glyph : glyphsIn) {
        spriteFontMap.emplace(glyph.character, glyph);
    }
    return nullptr;
}

std::unique_ptr<Error>
SpriteFont::Impl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn,
    const std::shared_ptr<TrueTypeFont>& fontIn,
    f32 fontSizeIn,
    f32 lineSpacingIn)
{
    lineSpacing = lineSpacingIn;
    fontSize = fontSizeIn;
    graphicsDevice = graphicsDeviceIn;
    font = fontIn;

    POMDOG_ASSERT(font);

    pixelData.resize(TextureWidth * TextureHeight, 0);
    currentPoint = {1, 1};
    bottomY = 1;

    if (auto [texture, textureErr] = graphicsDevice->createTexture2D(
            TextureWidth, TextureHeight, false, gpu::PixelFormat::A8_UNorm);
        textureErr != nullptr) {
        return errors::wrap(std::move(textureErr), "failed to create texture for sprite font");
    }
    else {
        // NOTE: Zero-initialize the GPU texture so that gap texels between glyphs
        // are transparent, preventing bilinear filtering artifacts at edges.
        texture->setData(pixelData.data());
        textures.push_back(std::move(texture));
    }

    return nullptr;
}

template <typename Func>
void SpriteFont::Impl::forEach(const std::string& text, Func func)
{
    auto position = Vector2::createZero();

    auto textIter = std::begin(text);
    const auto textEnd = std::end(text);

    while (textIter != textEnd) {
        const auto character = utf8::next(textIter, textEnd);

        switch (character) {
        case U'\n': {
            position.x = 0;
            position.y += lineSpacing;
            FontGlyph glyph;
            glyph.character = U'\n';
            glyph.subrect = Rect2D{0, 0, 0, 0};
            glyph.xAdvance = 0;
            func(glyph, position);
            break;
        }
        case U'\r': {
            break;
        }
        default: {
            POMDOG_ASSERT(character != 0x00);
            POMDOG_ASSERT(character != 0x08);
            POMDOG_ASSERT(character != 0x1B);

            auto iter = spriteFontMap.find(character);
            if (iter == std::end(spriteFontMap)) {
                // NOTE: Rasterize glyphs immediately
                prepareFonts(text);

                iter = spriteFontMap.find(character);
                if (iter == std::end(spriteFontMap)) {
                    iter = spriteFontMap.find(defaultCharacter);
                }
            }

            POMDOG_ASSERT(iter != std::end(spriteFontMap));
            if (iter == std::end(spriteFontMap)) {
                continue;
            }

            const auto& glyph = iter->second;
            position.x += static_cast<f32>(glyph.xOffset);

            func(glyph, position);

            const auto advance = glyph.xAdvance - glyph.xOffset;
            position.x += (static_cast<f32>(advance) - spacing);
            break;
        }
        }
    }
}

void SpriteFont::Impl::prepareFonts(const std::string& text)
{
    POMDOG_ASSERT(!text.empty());

    if (!graphicsDevice || !font) {
        return;
    }

    auto textIter = std::begin(text);
    auto textIterEnd = std::end(text);

    while (textIter != textIterEnd) {
        const auto character = utf8::next(textIter, textIterEnd);

        if (spriteFontMap.find(character) != std::end(spriteFontMap)) {
            continue;
        }

        auto glyph = font->rasterizeGlyph(character, fontSize, TextureWidth,
            [&](int glyphWidth, int glyphHeight, Point2D& pointOut, std::uint8_t*& output) {
                if (currentPoint.x + glyphWidth + 1 >= TextureWidth) {
                    // advance to next row
                    currentPoint.y = bottomY;
                    currentPoint.x = 1;
                }
                if (currentPoint.y + glyphHeight + 1 >= TextureHeight) {
                    std::fill(std::begin(pixelData), std::end(pixelData), static_cast<std::uint8_t>(0));

                    if (auto [textureNew, textureErr] = graphicsDevice->createTexture2D(
                            TextureWidth, TextureHeight, false, gpu::PixelFormat::A8_UNorm);
                        textureErr != nullptr) {
                        return;
                    }
                    else {
                        // NOTE: Zero-initialize the GPU texture
                        textureNew->setData(pixelData.data());
                        textures.push_back(std::move(textureNew));
                    }
                    currentPoint = {1, 1};
                    bottomY = 1;
                }

                POMDOG_ASSERT(currentPoint.x + glyphWidth < TextureWidth);
                POMDOG_ASSERT(currentPoint.y + glyphHeight < TextureHeight);

                pointOut = currentPoint;
                output = pixelData.data();
            });

        if (!glyph) {
            continue;
        }

        // NOTE: Upload only the glyph's rectangular region instead of the entire texture.
        if (glyph->subrect.width > 0 && glyph->subrect.height > 0) {
            // NOTE: Extract the glyph region from the atlas pixel buffer into a contiguous block.
            const auto regionWidth = glyph->subrect.width;
            const auto regionHeight = glyph->subrect.height;
            const auto regionX = glyph->subrect.x;
            const auto regionY = glyph->subrect.y;
            const u32 bytesPerRow = static_cast<u32>(regionWidth); // A8_UNorm = 1 byte per pixel

            std::vector<u8> glyphPixels(static_cast<std::size_t>(regionWidth) * regionHeight);
            for (int row = 0; row < regionHeight; ++row) {
                const auto srcOffset = (regionY + row) * TextureWidth + regionX;
                const auto dstOffset = row * regionWidth;
                std::memcpy(
                    glyphPixels.data() + dstOffset,
                    pixelData.data() + srcOffset,
                    regionWidth);
            }

            auto texture = textures.back();
            const auto region = Rect2D{regionX, regionY, regionWidth, regionHeight};
            texture->setData(0, region, glyphPixels.data(), bytesPerRow);
        }

        currentPoint.x = currentPoint.x + glyph->subrect.width + 1;
        bottomY = std::max(bottomY, currentPoint.y + glyph->subrect.height + 1);

        POMDOG_ASSERT(!textures.empty() && textures.size() > 0);
        glyph->texturePage = static_cast<std::int16_t>(textures.size()) - 1;

        spriteFontMap.emplace(glyph->character, *glyph);
    }
}

Vector2 SpriteFont::Impl::measureString(const std::string& text)
{
    POMDOG_ASSERT(!text.empty());

    auto result = Vector2::createZero();

    forEach(text, [&](const FontGlyph& glyph, const Vector2& postion) {
        if (glyph.character == U'\n') {
            result = math::max(result, postion + Vector2{0.0f, lineSpacing});
            return;
        }
        f32 w = static_cast<f32>(glyph.subrect.width);
        f32 h = static_cast<f32>(glyph.subrect.height);
        h = std::max(h, lineSpacing);

        if (glyph.character == U' ') {
            const auto advance = glyph.xAdvance - glyph.xOffset;
            w += (static_cast<f32>(advance) - spacing);
        }

        result = math::max(result, postion + Vector2{w, h});
    });

    return result;
}

void SpriteFont::Impl::draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    if (text.empty()) {
        return;
    }

    if (textures.empty()) {
        return;
    }

    if ((scale.x == 0.0f) || (scale.y == 0.0f)) {
        return;
    }

    // FIXME: Need to optimize layout calculation here.
    const auto labelSize = measureString(text);

    if ((labelSize.x < 1.0f) || (labelSize.y < 1.0f)) {
        return;
    }
    const auto baseOffset = labelSize * originPivot - Vector2{0.0f, labelSize.y - lineSpacing};

    forEach(text, [&](const FontGlyph& glyph, const Vector2& pos) {
        if (isSpace(glyph.character)) {
            // NOTE: Skip rendering
            return;
        }
        if ((glyph.subrect.width <= 0) || (glyph.subrect.height <= 0)) {
            // NOTE: Skip rendering
            return;
        }

        POMDOG_ASSERT(glyph.character != U'\n');
        POMDOG_ASSERT(glyph.texturePage >= 0);
        POMDOG_ASSERT(glyph.texturePage < static_cast<int>(textures.size()));

        const auto w = static_cast<f32>(glyph.subrect.width);
        const auto h = static_cast<f32>(glyph.subrect.height);

        auto offset = Vector2{
            pos.x,
            -pos.y - (static_cast<f32>(glyph.yOffset) + h)};
        offset = (baseOffset - offset) / Vector2{w, h};

        spriteBatch.draw(textures[glyph.texturePage], position, glyph.subrect, color, rotation, offset, scale);
    });
}

SpriteFont::SpriteFont()
    : impl(std::make_unique<Impl>())
{
}

std::unique_ptr<Error>
SpriteFont::initialize(
    std::vector<std::shared_ptr<gpu::Texture2D>>&& textures,
    const std::vector<FontGlyph>& glyphs,
    f32 spacing,
    f32 lineSpacing)
{
    return impl->initialize(std::move(textures), glyphs, spacing, lineSpacing);
}

std::unique_ptr<Error>
SpriteFont::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    f32 fontSize,
    f32 lineSpacing)
{
    return impl->initialize(graphicsDevice, font, fontSize, lineSpacing);
}

SpriteFont::~SpriteFont() = default;

void SpriteFont::prepareFonts(const std::string& utf8String)
{
    POMDOG_ASSERT(impl);
    return impl->prepareFonts(utf8String);
}

Vector2 SpriteFont::measureString(const std::string& utf8String) const
{
    POMDOG_ASSERT(impl);
    if (utf8String.empty()) {
        return Vector2::createZero();
    }
    return impl->measureString(utf8String);
}

char32_t SpriteFont::getDefaultCharacter() const
{
    POMDOG_ASSERT(impl);
    return impl->defaultCharacter;
}

void SpriteFont::setDefaultCharacter(char32_t character)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(containsCharacter(character));
    impl->defaultCharacter = character;
}

f32 SpriteFont::getLineSpacing() const
{
    POMDOG_ASSERT(impl);
    return impl->lineSpacing;
}

void SpriteFont::setLineSpacing(f32 lineSpacingIn)
{
    POMDOG_ASSERT(impl);
    impl->lineSpacing = lineSpacingIn;
}

bool SpriteFont::containsCharacter(char32_t character) const
{
    POMDOG_ASSERT(impl);
    return impl->spriteFontMap.find(character) != std::end(impl->spriteFontMap);
}

void SpriteFont::draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color)
{
    if (text.empty()) {
        return;
    }

    impl->prepareFonts(text);
    impl->draw(spriteBatch, text, position, color, 0.0f, Vector2{0.0f, 0.0f}, Vector2{1.0f, 1.0f});
}

void SpriteFont::draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    draw(spriteBatch, text, position, color, rotation, originPivot, Vector2{scale, scale});
}

void SpriteFont::draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    if (text.empty()) {
        return;
    }

    impl->prepareFonts(text);
    impl->draw(spriteBatch, text, position, color, rotation, originPivot, scale);
}

} // namespace pomdog
