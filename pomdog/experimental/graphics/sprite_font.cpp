// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_font.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/graphics/font_glyph.h"
#include "pomdog/experimental/graphics/sprite_batch.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/math/color.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

bool isSpace(char32_t c) noexcept
{
    return (c == U' ') || (c == U'\t');
}

} // namespace

// MARK: - SpriteFont::Impl

class SpriteFont::Impl final {
public:
    static constexpr int TextureWidth = 2048;
    static constexpr int TextureHeight = 2048;

    std::unordered_map<char32_t, FontGlyph> spriteFontMap;

    char32_t defaultCharacter;
    float lineSpacing;
    float spacing;
    float fontSize;

    Impl(
        std::vector<std::shared_ptr<gpu::Texture2D>>&& textures,
        const std::vector<FontGlyph>& glyphs,
        float spacing,
        float lineSpacing);

    Impl(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        float fontSize,
        float lineSpacing);

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
    Point2D currentPoint;
    int bottomY;
};

SpriteFont::Impl::Impl(
    std::vector<std::shared_ptr<gpu::Texture2D>>&& texturesIn,
    const std::vector<FontGlyph>& glyphsIn,
    float spacingIn,
    float lineSpacingIn)
    : defaultCharacter(U' ')
    , lineSpacing(lineSpacingIn)
    , spacing(spacingIn)
    , fontSize(0)
    , textures(std::move(texturesIn))
{
    for (auto& glyph : glyphsIn) {
        spriteFontMap.emplace(glyph.character, glyph);
    }
}

SpriteFont::Impl::Impl(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn,
    const std::shared_ptr<TrueTypeFont>& fontIn,
    float fontSizeIn,
    float lineSpacingIn)
    : defaultCharacter(U' ')
    , lineSpacing(lineSpacingIn)
    , spacing(0)
    , fontSize(fontSizeIn)
    , graphicsDevice(graphicsDeviceIn)
    , font(fontIn)
{
    POMDOG_ASSERT(font);

    pixelData.resize(TextureWidth * TextureHeight, 0);
    currentPoint = {1, 1};
    bottomY = 1;

    auto texture = std::get<0>(graphicsDevice->createTexture2D(
        TextureWidth, TextureHeight, false, PixelFormat::A8_UNorm));
    textures.push_back(texture);
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
            glyph.subrect = Rectangle{0, 0, 0, 0};
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
            position.x += static_cast<float>(glyph.xOffset);

            func(glyph, position);

            const auto advance = glyph.xAdvance - glyph.xOffset;
            position.x += (static_cast<float>(advance) - spacing);
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

    bool needToFetchPixelData = false;

    auto fetchTextureData = [&] {
        if (needToFetchPixelData) {
            auto texture = textures.back();
            texture->setData(pixelData.data());
            needToFetchPixelData = false;
        }
    };

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
                    fetchTextureData();
                    std::fill(std::begin(pixelData), std::end(pixelData), static_cast<std::uint8_t>(0));

                    auto textureNew = std::get<0>(graphicsDevice->createTexture2D(
                        TextureWidth, TextureHeight, false, PixelFormat::A8_UNorm));
                    textures.push_back(textureNew);
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

        currentPoint.x = currentPoint.x + glyph->subrect.width + 1;
        bottomY = std::max(bottomY, currentPoint.y + glyph->subrect.height + 1);

        POMDOG_ASSERT(!textures.empty() && textures.size() > 0);
        glyph->texturePage = static_cast<std::int16_t>(textures.size()) - 1;

        spriteFontMap.emplace(glyph->character, *glyph);
        needToFetchPixelData = true;
    }

    fetchTextureData();
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
        float w = static_cast<float>(glyph.subrect.width);
        float h = static_cast<float>(glyph.subrect.height);
        h = std::max(h, lineSpacing);

        if (glyph.character == U' ') {
            const auto advance = glyph.xAdvance - glyph.xOffset;
            w += (static_cast<float>(advance) - spacing);
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

        const auto w = static_cast<float>(glyph.subrect.width);
        const auto h = static_cast<float>(glyph.subrect.height);

        auto offset = Vector2{
            pos.x,
            -pos.y - (static_cast<float>(glyph.yOffset) + h)};
        offset = (baseOffset - offset) / Vector2{w, h};

        spriteBatch.draw(textures[glyph.texturePage], position, glyph.subrect, color, rotation, offset, scale);
    });
}

// MARK: - SpriteFont

SpriteFont::SpriteFont(
    std::vector<std::shared_ptr<gpu::Texture2D>>&& textures,
    const std::vector<FontGlyph>& glyphs,
    float spacing,
    float lineSpacing)
    : impl(std::make_unique<Impl>(std::move(textures), glyphs, spacing, lineSpacing))
{
}

SpriteFont::SpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    float fontSize,
    float lineSpacing)
    : impl(std::make_unique<Impl>(graphicsDevice, font, fontSize, lineSpacing))
{
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

float SpriteFont::getLineSpacing() const
{
    POMDOG_ASSERT(impl);
    return impl->lineSpacing;
}

void SpriteFont::setLineSpacing(float lineSpacingIn)
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
    float scale)
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
