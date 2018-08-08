// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace Pomdog {

struct FontGlyph;
class SpriteBatchRenderer;
class TrueTypeFont;

class POMDOG_EXPORT SpriteFont final {
public:
    SpriteFont(
        std::vector<std::shared_ptr<Texture2D>> && textures,
        const std::vector<FontGlyph>& glyphs,
        char32_t defaultCharacter,
        std::int16_t spacing,
        std::int16_t lineSpacing);

    SpriteFont(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        char32_t defaultCharacter,
        std::int16_t lineSpacing);

    ~SpriteFont();

    Vector2 MeasureString(const std::string& text) const;

    char32_t GetDefaultCharacter() const;

    void SetDefaultCharacter(char32_t character);

    float GetLineSpacing() const;

    void SetLineSpacing(float lineSpacing);

    bool ContainsCharacter(char32_t character) const;

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color);

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<float>& rotation,
        float scale);

    void Draw(
        SpriteBatchRenderer & spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<float>& rotation,
        const Vector2& scale);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
