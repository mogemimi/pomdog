// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct FontGlyph;
class SpriteBatch;
class TrueTypeFont;

// NOTE: SpriteFont uses the Cartesian coordinate system in which text is drawn.
// Also the `originPivot` represents a anchor point of text label.
// If you want to align the bottom-left corner, the pivot is set to (0, 0).
// If you want to align the top-right corner, the pivot is set to (1, 1).
//
//                             width
//              <----------------------------------->
//           (0, 1)                              (1, 1)
//        ^     +-----------------------------------+
//        |     |                                   |
// height |     |                *                  |
//        |     |            (0.5, 0.5)             |
//        v     +-----------------------------------+
//           (0, 0)                               (1, 0)

class POMDOG_EXPORT SpriteFont final {
public:
    SpriteFont(
        std::vector<std::shared_ptr<gpu::Texture2D>>&& textures,
        const std::vector<FontGlyph>& glyphs,
        float spacing,
        float lineSpacing);

    SpriteFont(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<TrueTypeFont>& font,
        float fontSize,
        float lineSpacing);

    ~SpriteFont();

    void prepareFonts(const std::string& text);

    [[nodiscard]] Vector2
    measureString(const std::string& text) const;

    [[nodiscard]] char32_t
    getDefaultCharacter() const;

    void setDefaultCharacter(char32_t character);

    [[nodiscard]] float
    getLineSpacing() const;

    void setLineSpacing(float lineSpacing);

    [[nodiscard]] bool
    containsCharacter(char32_t character) const;

    void draw(
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color);

    void draw(
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        float scale);

    void draw(
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog
