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
#include "pomdog/utility/concepts.h"
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

enum class TextureFetchPolicy : u8 {
    Immediate,
    Delayed,
};

class SpriteFontImpl final : public SpriteFont {
private:
    static constexpr int TextureWidth = 2048;
    static constexpr int TextureHeight = 2048;
    static constexpr int SDFPadding = 4;

private:
    std::unordered_map<char32_t, FontGlyph> spriteFontMap_;
    std::vector<std::shared_ptr<gpu::Texture2D>> textures_;
    std::vector<std::shared_ptr<TrueTypeFont>> fonts_;
    std::vector<f32> fontSizes_;
    std::vector<u8> pixelData_;
    Point2D currentPoint_ = {0, 0};
    char32_t defaultCharacter_ = U' ';
    f32 lineSpacing_ = 0.0f;
    f32 spacing_ = 0.0f;
    int bottomY_ = 0;
    bool needToFetchPixelData_ = false;
    bool sdf_ = false;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        f32 fontSize,
        f32 lineSpacing,
        bool sdf);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::vector<std::shared_ptr<TrueTypeFont>>& fonts,
        const std::vector<f32>& fontSizes,
        f32 lineSpacing,
        bool sdf);

    void prepareFonts(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) override;

    [[nodiscard]] Vector2
    measureString(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) override;

    [[nodiscard]] char32_t
    getDefaultCharacter() const override;

    void setDefaultCharacter(char32_t character) override;

    [[nodiscard]] f32
    getLineSpacing() const override;

    void setLineSpacing(f32 lineSpacing) override;

    [[nodiscard]] bool
    containsCharacter(char32_t character) const override;

    [[nodiscard]] u32
    getRasterizedGlyphCount() const noexcept override;

    void draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color) override;

    void draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) override;

    void draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) override;

private:
    void prepareFontsWithPolicy(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text,
        TextureFetchPolicy textureFetchPolicy);

    void fetchTextureData();

    template <typename Func>
        requires concepts::callable_as<Func, void(const FontGlyph& glyph, char32_t character, const Vector2& position)>
    void forEach(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text,
        Func&& func);

    void drawImpl(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);
};

std::unique_ptr<Error>
SpriteFontImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn,
    const std::shared_ptr<TrueTypeFont>& fontIn,
    f32 fontSizeIn,
    f32 lineSpacingIn,
    bool sdfIn)
{
    return initialize(
        graphicsDeviceIn,
        std::vector<std::shared_ptr<TrueTypeFont>>{fontIn},
        std::vector<f32>{fontSizeIn},
        lineSpacingIn,
        sdfIn);
}

std::unique_ptr<Error>
SpriteFontImpl::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::vector<std::shared_ptr<TrueTypeFont>>& fontsIn,
    const std::vector<f32>& fontSizesIn,
    f32 lineSpacingIn,
    bool sdfIn)
{
    if (fontsIn.size() != fontSizesIn.size()) {
        return errors::make("fonts and fontSizes must have the same size");
    }
    if (fontsIn.empty()) {
        return errors::make("fonts must not be empty");
    }

    lineSpacing_ = lineSpacingIn;
    fontSizes_ = fontSizesIn;
    fonts_ = fontsIn;
    sdf_ = sdfIn;
    needToFetchPixelData_ = false;

    pixelData_.resize(TextureWidth * TextureHeight, 0);
    currentPoint_ = {1, 1};
    bottomY_ = 1;

    if (auto [texture, textureErr] = graphicsDevice->createTexture2D(
            TextureWidth, TextureHeight, false, gpu::PixelFormat::R8_UNorm);
        textureErr != nullptr) {
        return errors::wrap(std::move(textureErr), "failed to create texture for sprite font");
    }
    else {
        // NOTE: Zero-initialize the GPU texture so that gap texels between glyphs
        // are transparent, preventing bilinear filtering artifacts at edges.
        texture->setData(pixelData_.data());
        textures_.push_back(std::move(texture));
    }

    return nullptr;
}

void SpriteFontImpl::fetchTextureData()
{
    if (!needToFetchPixelData_) {
        return;
    }

    auto texture = textures_.back();

    // NOTE: Upload the entire atlas texture.
    texture->setData(pixelData_.data());
    needToFetchPixelData_ = false;
}

template <typename Func>
    requires concepts::callable_as<Func, void(const FontGlyph& glyph, char32_t character, const Vector2& position)>
void SpriteFontImpl::forEach(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& text,
    Func&& func)
{
    auto position = Vector2::createZero();

    auto textIter = std::begin(text);
    const auto textEnd = std::end(text);

    while (textIter != textEnd) {
        const auto character = utf8::next(textIter, textEnd);

        switch (character) {
        case U'\n': {
            position.x = 0;
            position.y += lineSpacing_;

            FontGlyph glyph;
            glyph.character = U'\n';
            glyph.subrect = Rect2D{0, 0, 0, 0};
            glyph.xOffset = 0;
            glyph.yOffset = 0;
            glyph.xAdvance = 0;
            glyph.texturePage = 0;

            func(glyph, character, position);
            break;
        }
        case U'\r': {
            break;
        }
        default: {
            POMDOG_ASSERT(character != 0x00);
            POMDOG_ASSERT(character != 0x08);
            POMDOG_ASSERT(character != 0x1B);

            auto iter = spriteFontMap_.find(character);
            if (iter == std::end(spriteFontMap_)) {
                if (graphicsDevice != nullptr) {
                    // NOTE: Try to prepare font glyphs with delayed texture fetch
                    prepareFontsWithPolicy(graphicsDevice, text, TextureFetchPolicy::Delayed);
                }

                iter = spriteFontMap_.find(character);
                if (iter == std::end(spriteFontMap_)) {
                    iter = spriteFontMap_.find(defaultCharacter_);
                }
            }

            if (iter == std::end(spriteFontMap_)) {
                continue;
            }

            const auto& glyph = iter->second;
            position.x += static_cast<f32>(glyph.xOffset);

            func(glyph, character, position);

            const auto advance = glyph.xAdvance - glyph.xOffset;
            position.x += (static_cast<f32>(advance) - spacing_);
            break;
        }
        }
    }
}

void SpriteFontImpl::prepareFontsWithPolicy(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& text,
    TextureFetchPolicy textureFetchPolicy)
{
    POMDOG_ASSERT(!text.empty());

    if (!graphicsDevice || fonts_.empty()) {
        return;
    }

    auto textIter = std::begin(text);
    auto textIterEnd = std::end(text);

    while (textIter != textIterEnd) {
        const auto character = utf8::next(textIter, textIterEnd);

        if (spriteFontMap_.find(character) != std::end(spriteFontMap_)) {
            continue;
        }

        auto callback = [&](int glyphWidth, int glyphHeight, Point2D& pointOut, std::uint8_t*& output) {
            if (currentPoint_.x + glyphWidth + 1 >= TextureWidth) {
                // advance to next row
                currentPoint_.y = bottomY_;
                currentPoint_.x = 1;
            }
            if (currentPoint_.y + glyphHeight + 1 >= TextureHeight) {
                fetchTextureData();
                std::fill(std::begin(pixelData_), std::end(pixelData_), static_cast<std::uint8_t>(0));

                if (auto [textureNew, textureErr] = graphicsDevice->createTexture2D(
                        TextureWidth, TextureHeight, false, gpu::PixelFormat::R8_UNorm);
                    textureErr != nullptr) {
                    return;
                }
                else {
                    textures_.push_back(std::move(textureNew));
                }
                currentPoint_ = {1, 1};
                bottomY_ = 1;
            }

            POMDOG_ASSERT(currentPoint_.x + glyphWidth < TextureWidth);
            POMDOG_ASSERT(currentPoint_.y + glyphHeight < TextureHeight);

            pointOut = currentPoint_;
            output = pixelData_.data();
        };

        // NOTE: Try each font in order until one succeeds (font fallback).
        std::optional<FontGlyph> glyph;
        for (std::size_t fontIndex = 0; fontIndex < fonts_.size(); ++fontIndex) {
            glyph = fonts_[fontIndex]->rasterizeGlyph(
                character, fontSizes_[fontIndex], TextureWidth, TextureHeight, sdf_, SDFPadding, callback);
            if (glyph) {
                break;
            }
        }
        if (!glyph) {
            continue;
        }

        currentPoint_.x = currentPoint_.x + glyph->subrect.width + 1;
        bottomY_ = std::max(bottomY_, currentPoint_.y + glyph->subrect.height + 1);

        POMDOG_ASSERT(!textures_.empty() && textures_.size() > 0);
        glyph->texturePage = static_cast<std::int16_t>(textures_.size()) - 1;

        spriteFontMap_.emplace(glyph->character, *glyph);
        needToFetchPixelData_ = true;
    }

    if (textureFetchPolicy == TextureFetchPolicy::Immediate) {
        fetchTextureData();
    }
}

void SpriteFontImpl::prepareFonts(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& text)
{
    prepareFontsWithPolicy(graphicsDevice, text, TextureFetchPolicy::Immediate);
}

Vector2 SpriteFontImpl::measureString(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& text)
{
    if (text.empty()) {
        return Vector2::createZero();
    }

    auto result = Vector2::createZero();

    forEach(graphicsDevice, text, [&](const FontGlyph& glyph, char32_t character, const Vector2& postion) {
        if (character == U'\n') {
            result = math::max(result, postion + Vector2{0.0f, lineSpacing_});
            return;
        }

        // NOTE: SDF glyphs include extra padding around the bitmap for the distance field.
        // Use logical (unpadded) dimensions for measurement so that the measured text
        // size is consistent regardless of whether SDF rendering is enabled.
        const f32 pad = (sdf_ && !isSpace(character)) ? static_cast<f32>(SDFPadding) : 0.0f;
        f32 w = static_cast<f32>(glyph.subrect.width) - 2.0f * pad;
        f32 h = static_cast<f32>(glyph.subrect.height) - 2.0f * pad;
        h = std::max(h, lineSpacing_);

        if (character == U' ') {
            const auto advance = glyph.xAdvance - glyph.xOffset;
            w += (static_cast<f32>(advance) - spacing_);
        }

        result = math::max(result, postion + Vector2{pad + w, h});
    });

    return result;
}

char32_t SpriteFontImpl::getDefaultCharacter() const
{
    return defaultCharacter_;
}

void SpriteFontImpl::setDefaultCharacter(char32_t character)
{
    defaultCharacter_ = character;
}

f32 SpriteFontImpl::getLineSpacing() const
{
    return lineSpacing_;
}

void SpriteFontImpl::setLineSpacing(f32 lineSpacingIn)
{
    lineSpacing_ = lineSpacingIn;
}

bool SpriteFontImpl::containsCharacter(char32_t character) const
{
    return spriteFontMap_.find(character) != std::end(spriteFontMap_);
}

u32 SpriteFontImpl::getRasterizedGlyphCount() const noexcept
{
    return static_cast<u32>(spriteFontMap_.size());
}

void SpriteFontImpl::drawImpl(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
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

    fetchTextureData();

    if (textures_.empty()) {
        return;
    }

    if ((scale.x == 0.0f) || (scale.y == 0.0f)) {
        return;
    }

    // FIXME: Need to optimize layout calculation here.
    const auto labelSize = measureString(graphicsDevice, text);

    if ((labelSize.x <= 0.0f) || (labelSize.y <= 0.0f)) {
        return;
    }
    const auto baseOffset = labelSize * originPivot - Vector2{0.0f, labelSize.y - lineSpacing_};

    forEach(graphicsDevice, text, [&](const FontGlyph& glyph, char32_t character, const Vector2& pos) {
        if (isSpace(character)) {
            // NOTE: Skip rendering
            return;
        }
        if ((glyph.subrect.width <= 0) || (glyph.subrect.height <= 0)) {
            // NOTE: Skip rendering
            return;
        }

        POMDOG_ASSERT(character != U'\n');
        POMDOG_ASSERT(glyph.texturePage >= 0);
        POMDOG_ASSERT(glyph.texturePage < static_cast<int>(textures_.size()));

        const auto w = static_cast<f32>(glyph.subrect.width);
        const auto h = static_cast<f32>(glyph.subrect.height);

        auto offset = Vector2{
            pos.x,
            -pos.y - (static_cast<f32>(glyph.yOffset) + h)};
        offset = (baseOffset - offset) / Vector2{w, h};

        spriteBatch.draw(textures_[glyph.texturePage], position, glyph.subrect, color, rotation, offset, scale);
    });
}

void SpriteFontImpl::draw(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color)
{
    if (text.empty()) {
        return;
    }

    drawImpl(
        graphicsDevice,
        spriteBatch,
        text,
        position,
        color,
        0.0f,
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f});
}

void SpriteFontImpl::draw(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    SpriteBatch& spriteBatch,
    const std::string& text,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    f32 scale)
{
    if (text.empty()) {
        return;
    }

    drawImpl(
        graphicsDevice,
        spriteBatch,
        text,
        position,
        color,
        rotation,
        originPivot,
        Vector2{scale, scale});
}

void SpriteFontImpl::draw(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
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

    drawImpl(
        graphicsDevice,
        spriteBatch,
        text,
        position,
        color,
        rotation,
        originPivot,
        scale);
}

} // namespace

SpriteFont::~SpriteFont() = default;

std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    f32 fontSize,
    f32 lineSpacing,
    bool sdf) noexcept
{
    auto spriteFont = std::make_shared<SpriteFontImpl>();
    if (auto err = spriteFont->initialize(graphicsDevice, font, fontSize, lineSpacing, sdf);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(spriteFont), nullptr);
}

std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::vector<std::shared_ptr<TrueTypeFont>>& fonts,
    const std::vector<f32>& fontSizes,
    f32 lineSpacing,
    bool sdf) noexcept
{
    auto spriteFont = std::make_shared<SpriteFontImpl>();
    if (auto err = spriteFont->initialize(graphicsDevice, fonts, fontSizes, lineSpacing, sdf);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(spriteFont), nullptr);
}

} // namespace pomdog
