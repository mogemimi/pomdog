// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// Holds the SDF shader parameters produced by computeSpriteFontSDFParameters.
///
/// Copy these into SpriteFontDrawParameters or SpriteBatchDrawDistanceField.
/// The fontSmoothing and fontWeight always satisfy `fontSmoothing + fontWeight`
/// at or below 1.0 so that zero-valued atlas padding stays transparent.
struct POMDOG_EXPORT SpriteFontSDFParameters final {
    /// Controls the amount of SDF font smoothing (0.0 to 1.0).
    ///
    /// Values near 0.0 produce sharper edges. Values near 1.0 produce softer,
    /// blurrier edges.
    f32 fontSmoothing = 0.140f;

    /// Controls the SDF font weight from thin (0.0) to thick (1.0).
    f32 fontWeight = 0.560f;

    /// Controls the SDF outline threshold (0.0 to 1.0).
    ///
    /// Values near 0.0 produce a thinner outline. Values near 1.0 produce a
    /// thicker outline. Only used by the DistanceFieldWithOutline shader.
    f32 outlineWeight = 0.440f;
};

/// Describes the rendering condition and the desired look of SDF text.
///
/// computeSpriteFontSDFParameters turns this description into shader
/// parameters. Stroke and outline adjustments are expressed in physical screen
/// pixels, so the same request keeps a consistent look across font sizes, draw
/// scales, and DPI scales.
struct POMDOG_EXPORT SpriteFontSDFDesc final {
    /// The font size, in pixels, passed to createSpriteFont.
    ///
    /// Together with effectiveScale this gives the on-screen glyph height, which
    /// scales the default stroke weight: smaller text renders thinner and text
    /// drawn at a larger scale renders slightly heavier.
    f32 fontSize = 32.0f;

    /// The number of physical screen pixels covered by one atlas pixel.
    ///
    /// For text laid out in logical pixels with a uniform draw scale, pass
    /// `drawScale * pixelRatio * uiScale`. Pass `drawScale * uiScale` when the
    /// projection and text positions are already in physical pixels.
    f32 effectiveScale = 1.0f;

    /// Adjusts the stroke thickness in physical screen pixels.
    ///
    /// Zero keeps the default look. Positive values make glyphs
    /// thicker and negative values make them thinner. A magnitude of 0.5 pixels
    /// is a subtle change and 1.0 pixel is a strong change.
    f32 weight = 0.0f;

    /// Sets the outline width in physical screen pixels.
    ///
    /// Zero disables the outline and selects plain glyph rendering for the
    /// DistanceField shader. A positive value wraps an outline of that width
    /// around the glyph for the DistanceFieldWithOutline shader and sets the
    /// returned outlineWeight accordingly.
    f32 outlineThickness = 0.0f;
};

/// Returns scale-aware SDF shader parameters for the given description.
///
/// This is an optional convenience helper that lives outside SpriteFont.
/// SpriteFont renders SDF text from any valid SpriteFontDrawParameters, so an
/// application may compute equivalent values itself instead of calling this.
///
/// The result targets a constant screen-space antialiasing width, so text stays
/// equally sharp at any size, and a stroke weight that grows with the on-screen
/// glyph size, so small text stays thin and large text renders slightly
/// heavier. The mapping is derived from the SDF atlas constants rather than a
/// measured table, so it has no allocations or loops and is suitable for
/// per-frame use. Invalid fontSize or effectiveScale values fall back to safe
/// defaults, and the returned parameters keep zero-valued atlas
/// padding transparent.
[[nodiscard]] POMDOG_EXPORT SpriteFontSDFParameters
computeSpriteFontSDFParameters(const SpriteFontSDFDesc& desc) noexcept;

} // namespace pomdog
