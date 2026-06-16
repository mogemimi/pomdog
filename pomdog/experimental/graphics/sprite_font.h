// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class SpriteBatch;
class TrueTypeFont;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog {

/// Provides parameters for drawing text with SpriteFont.
struct POMDOG_EXPORT SpriteFontDrawParameters final {
    /// Specifies the text color.
    Color color = Color::createWhite();

    /// Specifies the color used for font outlines.
    Color outlineColor = Color::createTransparentBlack();

    /// Controls the amount of SDF font smoothing (0.0 to 1.0).
    ///
    /// Values near 0.0 produce sharper edges. Values near 1.0 produce softer,
    /// blurrier edges.
    ///
    /// Keep `fontSmoothing + fontWeight` at or below 1.0 so that zero-valued SDF
    /// padding stays transparent; larger sums can reveal the glyph quad edges.
    f32 fontSmoothing = 0.140f;

    /// Controls the SDF font weight from thin (0.0) to thick (1.0).
    ///
    /// Keep `fontSmoothing + fontWeight` at or below 1.0. This is not enforced
    /// at draw time, but computeSpriteFontSDFParameters guarantees it.
    f32 fontWeight = 0.560f;

    /// Controls the SDF outline threshold (0.0 to 1.0).
    ///
    /// Values near 0.0 produce a thinner outline. Values near 1.0 produce a
    /// thicker outline.
    f32 outlineWeight = 0.440f;

    /// Specifies the rotation of the text in radians.
    Radian<f32> rotation = Radian<f32>{0.0f};

    /// Specifies the origin pivot point for scaling and rotation.
    ///
    /// The bottom-left corner is (0, 0), the top-right corner is (1, 1),
    /// and the center is (0.5, 0.5).
    Vector2 originPivot = Vector2{0.0f, 0.0f};

    /// Specifies the scaling factor for the text.
    Vector2 scale = Vector2{1.0f, 1.0f};

    /// Specifies the depth of the text used for layering.
    f32 layerDepth = 0.0f;
};

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

    /// Pre-rasterizes the glyphs for text and uploads them to the atlas.
    ///
    /// Rasterizing happens on the CPU, but transferring glyphs to the atlas
    /// texture is a GPU operation, so graphicsDevice is required.
    virtual void
    prepareFonts(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) = 0;

    /// Returns the width and height required to draw text.
    ///
    /// When graphicsDevice is non-null, any glyphs that are not yet cached are
    /// rasterized on demand and uploaded to the atlas texture. Pass nullptr to
    /// avoid that GPU side effect and measure using only the cached glyphs, for
    /// example when measuring text without drawing it.
    [[nodiscard]] virtual Vector2
    measureString(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::string& text) = 0;

    /// Returns the default character used when a glyph is not found.
    [[nodiscard]] virtual char32_t
    getDefaultCharacter() const = 0;

    /// Sets the fallback character drawn when a glyph is not found.
    ///
    /// The fallback character must already be rasterized.
    virtual void
    setDefaultCharacter(char32_t character) = 0;

    /// Returns the vertical distance between baselines of consecutive lines.
    [[nodiscard]] virtual f32
    getLineSpacing() const = 0;

    /// Sets the vertical distance in pixels between baselines of consecutive lines.
    virtual void
    setLineSpacing(f32 lineSpacing) = 0;

    /// Returns whether the specified character has been rasterized and cached.
    [[nodiscard]] virtual bool
    containsCharacter(char32_t character) const = 0;

    /// Returns the number of glyphs that have been rasterized and cached.
    [[nodiscard]] virtual u32
    getRasterizedGlyphCount() const noexcept = 0;

    /// Draws text using the specified parameters.
    ///
    /// Uses graphicsDevice for on-demand glyph rasterization.
    virtual void
    draw(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        SpriteBatch& spriteBatch,
        const std::string& text,
        const Vector2& position,
        const SpriteFontDrawParameters& params) = 0;
};

/// Creates a SpriteFont that rasterizes glyphs from font on demand.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<TrueTypeFont>& font,
    f32 fontSize,
    f32 lineSpacing,
    bool sdf) noexcept;

/// Creates a SpriteFont from several fonts that are tried in order per glyph.
///
/// Each font uses the matching entry in fontSizes. When a glyph is missing from
/// the first font, the following fonts are tried as fallbacks. See the
/// single-font overload for the meaning of fontSize and sdf.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<SpriteFont>, std::unique_ptr<Error>>
createSpriteFont(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::vector<std::shared_ptr<TrueTypeFont>>& fonts,
    const std::vector<f32>& fontSizes,
    f32 lineSpacing,
    bool sdf) noexcept;

} // namespace pomdog
