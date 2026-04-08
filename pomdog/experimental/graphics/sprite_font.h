// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Color;
class SpriteBatch;
class TrueTypeFont;
class Vector2;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

/// Represents a bitmap font that can be drawn using SpriteBatch.
///
/// SpriteFont uses the Cartesian coordinate system in which text is drawn.
/// Also the `originPivot` represents a anchor point of text label.
/// If you want to align the bottom-left corner, the pivot is set to (0, 0).
/// If you want to align the top-right corner, the pivot is set to (1, 1).
/// ```
///                           width
///              ◄───────────────────────────────►
///           (0, 1)                          (1, 1)
///        ▲     ┌───────────────────────────────┐
///        │     │                               │
/// height │     │              *                │
///        │     │          (0.5, 0.5)           │
///        ▼     └───────────────────────────────┘
///           (0, 0)                           (1, 0)
/// ```
class POMDOG_EXPORT SpriteFont {
public:
    virtual ~SpriteFont();

    /// Pre-rasterizes font glyphs for the given text.
    ///
    /// Rasterizing is a CPU-side operation, but transferring glyphs to the atlas
    /// texture requires a GPU-side operation, so a graphics device is passed.
    ///
    /// @param graphicsDevice The graphics device used for texture uploads.
    /// @param text The string whose glyphs should be rasterized and cached.
    virtual void
    prepareFonts(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) = 0;

    /// Measures the size required to draw the given string.
    ///
    /// If the text contains glyphs that have not yet been rasterized, they will be
    /// rasterized on-demand using the given graphics device.
    ///
    /// @param graphicsDevice The graphics device used for texture uploads.
    /// @param text The string to measure.
    /// @return The width and height of the rendered text.
    [[nodiscard]] virtual Vector2
    measureString(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) = 0;

    /// Returns the default character used when a glyph is not found.
    [[nodiscard]] virtual char32_t
    getDefaultCharacter() const = 0;

    /// Sets the default character used when a glyph is not found.
    ///
    /// @param character The fallback character (must already be rasterized).
    virtual void
    setDefaultCharacter(char32_t character) = 0;

    /// Returns the vertical distance between baselines of consecutive lines.
    [[nodiscard]] virtual f32
    getLineSpacing() const = 0;

    /// Sets the vertical distance between baselines of consecutive lines.
    ///
    /// @param lineSpacing Line spacing in pixels.
    virtual void
    setLineSpacing(f32 lineSpacing) = 0;

    /// Returns whether the specified character has been rasterized and cached.
    [[nodiscard]] virtual bool
    containsCharacter(char32_t character) const = 0;

    /// Returns the number of glyphs that have been rasterized and cached.
    [[nodiscard]] virtual u32
    getRasterizedGlyphCount() const noexcept = 0;

    /// Draws text at the given position with default rotation, origin, and scale.
    ///
    /// @param graphicsDevice The graphics device used for on-demand glyph rasterization.
    /// @param spriteBatch The sprite batch to draw with.
    /// @param text The string to draw.
    /// @param position The position of the text.
    /// @param color The text color.
    virtual void
    draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color) = 0;

    /// Draws text with uniform scale.
    ///
    /// @param graphicsDevice The graphics device used for on-demand glyph rasterization.
    /// @param spriteBatch The sprite batch to draw with.
    /// @param text The string to draw.
    /// @param position The position of the text.
    /// @param color The text color.
    /// @param rotation Rotation angle in radians.
    /// @param originPivot Origin pivot (0..1 per axis).
    /// @param scale Uniform scale factor.
    virtual void
    draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        f32 scale) = 0;

    /// Draws text with non-uniform scale.
    ///
    /// @param graphicsDevice The graphics device used for on-demand glyph rasterization.
    /// @param spriteBatch The sprite batch to draw with.
    /// @param text The string to draw.
    /// @param position The position of the text.
    /// @param color The text color.
    /// @param rotation Rotation angle in radians.
    /// @param originPivot Origin pivot (0..1 per axis).
    /// @param scale Scale factors per axis.
    virtual void
    draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const Color& color,
        const Radian<f32>& rotation,
        const Vector2& originPivot,
        const Vector2& scale) = 0;
};

/// Creates a SpriteFont instance.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    f32 fontSize,
    f32 lineSpacing,
    bool sdf) noexcept;

/// Creates a SpriteFont instance with multiple fonts (font fallback).
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::vector<std::shared_ptr<TrueTypeFont>>& fonts,
    const std::vector<f32>& fontSizes,
    f32 lineSpacing,
    bool sdf) noexcept;

} // namespace pomdog
