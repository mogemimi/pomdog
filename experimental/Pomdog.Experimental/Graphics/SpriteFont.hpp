// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP
#define POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
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
    std::uint8_t TexturePage;
};

}// namespace SpriteFonts
}// namespace Detail

class SpriteBatch;

class SpriteFont final {
public:
    SpriteFont(std::vector<std::shared_ptr<Texture2D>> && textures,
        std::vector<Detail::SpriteFonts::Glyph> const& glyphs,
        std::uint32_t defaultCharacter,
        std::int16_t spacing,
        std::int16_t lineSpacing);

    ~SpriteFont();

    Vector2 MeasureString(char const* text) const;

    //Vector2 MeasureString(std::string const& text) const;

    std::uint32_t DefaultCharacter() const;

    void DefaultCharacter(std::uint32_t character);

    float LineSpacing() const;

    void LineSpacing(float lineSpacing);

    bool ContainsCharacter(std::uint32_t character) const;

    void Begin(Matrix4x4 const& transformMatrix);

    void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color);

    void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, float scale, float layerDepth);

    void Draw(SpriteBatch & spriteBatch, std::string const& text, Vector2 const& position, Color const& color,
        Radian<float> const& rotation, Vector2 const& originPivot, Vector2 const& scale, float layerDepth);

    void End();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEFONT_1C2133CC_E7F0_4FC2_A4D8_2D2862E4B3C6_HPP)
