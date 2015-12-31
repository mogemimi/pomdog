// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace Pomdog {
namespace Detail {
namespace SpriteFonts {

struct Glyph {
    Rectangle Subrect;

    ///@note UTF-32
    std::uint32_t Character;

    std::int16_t XOffset;
    std::int16_t YOffset;
    std::int16_t XAdvance;
    std::int16_t TexturePage;
};

} // namespace SpriteFonts
} // namespace Detail

class SpriteBatchRenderer;
class TrueTypeFont;

class SpriteFont final {
public:
    SpriteFont(std::vector<std::shared_ptr<Texture2D>> && textures,
        std::vector<Detail::SpriteFonts::Glyph> const& glyphs,
        std::uint32_t defaultCharacter,
        std::int16_t spacing,
        std::int16_t lineSpacing);

    SpriteFont(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        std::shared_ptr<TrueTypeFont> const& font,
        std::uint32_t defaultCharacter,
        std::int16_t lineSpacing);

    ~SpriteFont();

    Vector2 MeasureString(std::string const& text) const;

    std::uint32_t GetDefaultCharacter() const;

    void SetDefaultCharacter(std::uint32_t character);

    float GetLineSpacing() const;

    void SetLineSpacing(float lineSpacing);

    bool ContainsCharacter(std::uint32_t character) const;

    void Begin(
        std::shared_ptr<GraphicsCommandList> const& commandList,
        SpriteBatchRenderer & spriteBatch,
        Matrix4x4 const& transformMatrix);

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        std::string const& text,
        Vector2 const& position,
        Color const& color);

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        std::string const& text,
        Vector2 const& position,
        Color const& color,
        Radian<float> const& rotation,
        float scale);

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        std::string const& text,
        Vector2 const& position,
        Color const& color,
        Radian<float> const& rotation,
        Vector2 const& scale);

    void End(SpriteBatchRenderer & spriteBatch);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
