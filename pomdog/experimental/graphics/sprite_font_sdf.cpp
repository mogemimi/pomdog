// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/graphics/sprite_font_sdf.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

[[nodiscard]] f32
sanitizeEffectiveScale(f32 effectiveScale) noexcept
{
    if (!std::isfinite(effectiveScale) || (effectiveScale <= 0.0f)) {
        return 1.0f;
    }
    return effectiveScale;
}

[[nodiscard]] f32
clampSDFFontWeight(f32 fontWeight, f32 fontSmoothing) noexcept
{
    // NOTE: SpriteBatch sends `1 - fontWeight` to the shader. Keeping the
    // lower edge of smoothstep at or above the zero-valued atlas padding
    // requires `fontSmoothing + fontWeight <= 1`.
    const auto smoothing = std::clamp(fontSmoothing, 0.0f, 1.0f);
    return std::clamp(fontWeight, 0.0f, 1.0f - smoothing);
}

} // namespace

SpriteFontSDFParameters
computeSpriteFontSDFParameters(const SpriteFontSDFDesc& desc) noexcept
{
    // NOTE: TrueTypeFontImpl writes the glyph edge at 128/255 and changes the
    // stored distance by 48/255 per atlas pixel (pixel distance scale 48).
    // These constants convert a physical screen-pixel distance into the
    // normalized SDF space that the shader thresholds operate in.
    constexpr auto onEdgeValue = 128.0f / 255.0f;
    constexpr auto normPerAtlasPixel = 48.0f / 255.0f;

    // NOTE: Reference on-screen size. A glyph this tall on screen renders at
    // the default fill weight (about 0.56) when effectiveScale is 1.0.
    constexpr auto referenceOnScreenPixels = 32.0f;

    // NOTE: Target antialiasing transition width in physical screen pixels. The
    // band is kept constant in screen space so text stays equally sharp at any
    // size.
    constexpr auto antialiasBandPixels = 1.25f;

    // NOTE: Default outward stroke bias, in physical screen pixels, at the
    // reference on-screen size. It is scaled by the on-screen size below, so
    // small text renders thin and text drawn at a large scale renders slightly
    // heavier.
    constexpr auto strokeBiasReferencePixels = 0.33f;

    // NOTE: Bounds for the on-screen size factor that scales the stroke bias.
    // The lower bound (no floor) keeps small text thin; the upper bound caps how
    // heavy very large text becomes.
    constexpr auto boldnessFloor = 0.0f;
    constexpr auto boldnessCap = 1.5f;

    constexpr auto minSmoothing = 0.01f;
    constexpr auto maxSmoothing = 0.25f;

    const auto fontSize = (std::isfinite(desc.fontSize) && (desc.fontSize > 0.0f))
                              ? desc.fontSize
                              : referenceOnScreenPixels;
    const auto effectiveScale = sanitizeEffectiveScale(desc.effectiveScale);

    // NOTE: Normalized SDF change per physical screen pixel.
    const auto normPerScreenPixel = normPerAtlasPixel / effectiveScale;

    // NOTE: Antialiasing targets a constant screen-space band, so smoothing
    // depends only on the screen scale and not on the font size.
    const auto smoothing = std::clamp(
        0.5f * antialiasBandPixels * normPerScreenPixel,
        minSmoothing,
        maxSmoothing);

    // NOTE: The default stroke bias scales with the on-screen glyph size, so
    // smaller text stays thin and text drawn at a large scale renders slightly
    // heavier.
    const auto onScreenPixels = fontSize * effectiveScale;
    const auto boldnessFactor = std::clamp(
        onScreenPixels / referenceOnScreenPixels, boldnessFloor, boldnessCap);

    // NOTE: Push the fill edge outside the geometric edge by the default bias
    // plus the caller weight. The default bias is referenced to the atlas and
    // scaled by the on-screen size, while the caller weight is a fixed
    // screen-pixel knob.
    const auto fillThreshold = onEdgeValue -
                               strokeBiasReferencePixels * normPerAtlasPixel * boldnessFactor -
                               desc.weight * normPerScreenPixel;

    SpriteFontSDFParameters result = {};
    result.fontSmoothing = smoothing;
    // NOTE: The outline-to-fill boundary sits at the fill edge. Unused without an
    // outline, but kept meaningful for callers that read it.
    result.outlineWeight = std::clamp(fillThreshold, 0.0f, 1.0f);

    if (desc.outlineThickness > 0.0f) {
        // NOTE: The outline extends outward from the fill edge. The visible
        // opacity edge (1 - fontWeight) moves out by the outline width so that
        // the outline wraps the glyph.
        const auto opacityThreshold = fillThreshold - desc.outlineThickness * normPerScreenPixel;
        result.fontWeight = 1.0f - opacityThreshold;
    }
    else {
        result.fontWeight = 1.0f - fillThreshold;
    }

    result.fontWeight = clampSDFFontWeight(result.fontWeight, result.fontSmoothing);
    return result;
}

} // namespace pomdog
