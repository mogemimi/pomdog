// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_font.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/graphics/font_glyph.hpp"
#include "pomdog/experimental/graphics/sprite_batch.hpp"
#include "pomdog/experimental/graphics/truetype_font.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/texture2d.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/math/matrix3x2.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/point2d.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utfcpp/source/utf8.h>
#include <algorithm>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
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
        std::vector<std::shared_ptr<Texture2D>>&& textures,
        const std::vector<FontGlyph>& glyphs,
        float spacing,
        float lineSpacing);

    Impl(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        float fontSize,
        float lineSpacing);

    template <typename Func>
    void ForEach(const std::string& text, Func func);

    Vector2 MeasureString(const std::string& text);

    void Draw(
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

    void PrepareFonts(const std::string& text);

private:
    std::vector<std::shared_ptr<Texture2D>> textures;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::shared_ptr<TrueTypeFont> font;
    std::vector<std::uint8_t> pixelData;
    Point2D currentPoint;
    int bottomY;
};

SpriteFont::Impl::Impl(
    std::vector<std::shared_ptr<Texture2D>>&& texturesIn,
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
        spriteFontMap.emplace(glyph.Character, glyph);
    }
}

SpriteFont::Impl::Impl(
    const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn,
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

    auto texture = std::get<0>(graphicsDevice->CreateTexture2D(
        TextureWidth, TextureHeight, false, SurfaceFormat::A8_UNorm));
    textures.push_back(texture);
}

template <typename Func>
void SpriteFont::Impl::ForEach(const std::string& text, Func func)
{
    Vector2 position = Vector2::Zero;

    auto textIter = std::begin(text);
    const auto textEnd = std::end(text);

    while (textIter != textEnd) {
        const auto character = utf8::next(textIter, textEnd);

        switch (character) {
        case U'\n': {
            position.X = 0;
            position.Y += lineSpacing;
            FontGlyph glyph;
            glyph.Character = U'\n';
            glyph.Subrect = Rectangle{0, 0, 0, 0};
            glyph.XAdvance = 0;
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
                PrepareFonts(text);

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
            position.X += static_cast<float>(glyph.XOffset);

            func(glyph, position);

            const auto advance = glyph.XAdvance - glyph.XOffset;
            position.X += (static_cast<float>(advance) - spacing);
            break;
        }
        }
    }
}

void SpriteFont::Impl::PrepareFonts(const std::string& text)
{
    POMDOG_ASSERT(!text.empty());

    if (!graphicsDevice || !font) {
        return;
    }

    bool needToFetchPixelData = false;

    auto fetchTextureData = [&] {
        if (needToFetchPixelData) {
            auto texture = textures.back();
            texture->SetData(pixelData.data());
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

        auto glyph = font->RasterizeGlyph(character, fontSize, TextureWidth,
            [&](int glyphWidth, int glyphHeight, Point2D& pointOut, std::uint8_t*& output) {
                if (currentPoint.X + glyphWidth + 1 >= TextureWidth) {
                    // advance to next row
                    currentPoint.Y = bottomY;
                    currentPoint.X = 1;
                }
                if (currentPoint.Y + glyphHeight + 1 >= TextureHeight) {
                    fetchTextureData();
                    std::fill(std::begin(pixelData), std::end(pixelData), static_cast<std::uint8_t>(0));

                    auto textureNew = std::get<0>(graphicsDevice->CreateTexture2D(
                        TextureWidth, TextureHeight, false, SurfaceFormat::A8_UNorm));
                    textures.push_back(textureNew);
                    currentPoint = {1, 1};
                    bottomY = 1;
                }

                POMDOG_ASSERT(currentPoint.X + glyphWidth < TextureWidth);
                POMDOG_ASSERT(currentPoint.Y + glyphHeight < TextureHeight);

                pointOut = currentPoint;
                output = pixelData.data();
            });

        if (!glyph) {
            continue;
        }

        currentPoint.X = currentPoint.X + glyph->Subrect.Width + 1;
        bottomY = std::max(bottomY, currentPoint.Y + glyph->Subrect.Height + 1);

        POMDOG_ASSERT(!textures.empty() && textures.size() > 0);
        glyph->TexturePage = static_cast<std::int16_t>(textures.size()) - 1;

        spriteFontMap.emplace(glyph->Character, *glyph);
        needToFetchPixelData = true;
    }

    fetchTextureData();
}

Vector2 SpriteFont::Impl::MeasureString(const std::string& text)
{
    POMDOG_ASSERT(!text.empty());

    Vector2 result = Vector2::Zero;

    ForEach(text, [&](const FontGlyph& glyph, const Vector2& postion) {
        if (glyph.Character == U'\n') {
            result = Vector2::Max(result, postion + Vector2{0.0f, lineSpacing});
            return;
        }
        float w = static_cast<float>(glyph.Subrect.Width);
        float h = static_cast<float>(glyph.Subrect.Height);
        h = std::max(h, lineSpacing);

        if (glyph.Character == U' ') {
            const auto advance = glyph.XAdvance - glyph.XOffset;
            w += (static_cast<float>(advance) - spacing);
        }

        result = Vector2::Max(result, postion + Vector2{w, h});
    });

    return result;
}

void SpriteFont::Impl::Draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    if (text.empty()) {
        return;
    }

    if (textures.empty()) {
        return;
    }

    if ((scale.X == 0.0f) || (scale.Y == 0.0f)) {
        return;
    }

    // FIXME: Need to optimize layout calculation here.
    const auto labelSize = MeasureString(text);

    if ((labelSize.X < 1.0f) || (labelSize.Y < 1.0f)) {
        return;
    }
    const auto baseOffset = labelSize * originPivot - Vector2{0.0f, labelSize.Y - lineSpacing};

    ForEach(text, [&](const FontGlyph& glyph, const Vector2& pos) {
        if (isSpace(glyph.Character)) {
            // NOTE: Skip rendering
            return;
        }
        if ((glyph.Subrect.Width <= 0) || (glyph.Subrect.Height <= 0)) {
            // NOTE: Skip rendering
            return;
        }

        POMDOG_ASSERT(glyph.Character != U'\n');
        POMDOG_ASSERT(glyph.TexturePage >= 0);
        POMDOG_ASSERT(glyph.TexturePage < static_cast<int>(textures.size()));

        auto w = static_cast<float>(glyph.Subrect.Width);
        auto h = static_cast<float>(glyph.Subrect.Height);

        auto offset = Vector2{
            pos.X,
            -pos.Y - (static_cast<float>(glyph.YOffset) + h)};
        offset = (baseOffset - offset) / Vector2{w, h};

        spriteBatch.Draw(textures[glyph.TexturePage], position, glyph.Subrect, color, rotation, offset, scale);
    });
}

// MARK: - SpriteFont

SpriteFont::SpriteFont(
    std::vector<std::shared_ptr<Texture2D>>&& textures,
    const std::vector<FontGlyph>& glyphs,
    float spacing,
    float lineSpacing)
    : impl(std::make_unique<Impl>(std::move(textures), glyphs, spacing, lineSpacing))
{
}

SpriteFont::SpriteFont(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    float fontSize,
    float lineSpacing)
    : impl(std::make_unique<Impl>(graphicsDevice, font, fontSize, lineSpacing))
{
}

SpriteFont::~SpriteFont() = default;

void SpriteFont::PrepareFonts(const std::string& utf8String)
{
    POMDOG_ASSERT(impl);
    return impl->PrepareFonts(utf8String);
}

Vector2 SpriteFont::MeasureString(const std::string& utf8String) const
{
    POMDOG_ASSERT(impl);
    if (utf8String.empty()) {
        return Vector2::Zero;
    }
    return impl->MeasureString(utf8String);
}

char32_t SpriteFont::GetDefaultCharacter() const
{
    POMDOG_ASSERT(impl);
    return impl->defaultCharacter;
}

void SpriteFont::SetDefaultCharacter(char32_t character)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(ContainsCharacter(character));
    impl->defaultCharacter = character;
}

float SpriteFont::GetLineSpacing() const
{
    POMDOG_ASSERT(impl);
    return impl->lineSpacing;
}

void SpriteFont::SetLineSpacing(float lineSpacingIn)
{
    POMDOG_ASSERT(impl);
    impl->lineSpacing = lineSpacingIn;
}

bool SpriteFont::ContainsCharacter(char32_t character) const
{
    POMDOG_ASSERT(impl);
    return impl->spriteFontMap.find(character) != std::end(impl->spriteFontMap);
}

void SpriteFont::Draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color)
{
    if (text.empty()) {
        return;
    }

    impl->PrepareFonts(text);
    impl->Draw(spriteBatch, text, position, color, 0.0f, Vector2{0.0f, 0.0f}, Vector2{1.0f, 1.0f});
}

void SpriteFont::Draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    this->Draw(spriteBatch, text, position, color, rotation, originPivot, Vector2{scale, scale});
}

void SpriteFont::Draw(
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    const Vector2& scale)
{
    if (text.empty()) {
        return;
    }

    impl->PrepareFonts(text);
    impl->Draw(spriteBatch, text, position, color, rotation, originPivot, scale);
}

} // namespace Pomdog
