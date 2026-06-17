// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/graphics/sprite_font_sdf.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
#include <initializer_list>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::computeSpriteFontSDFParameters;
using pomdog::f32;
using pomdog::SpriteFontSDFDesc;

namespace {
auto approx(f32 value) -> auto
{
    return doctest::Approx(static_cast<double>(value)).epsilon(0.000001);
}

bool within(f32 value, f32 expected, f32 tolerance)
{
    return std::abs(value - expected) <= tolerance;
}
} // namespace

TEST_CASE("sprite_font_sdf_parameters")
{
    SUBCASE("returns the reference values at fontSize 32 and scale one")
    {
        const auto p = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        REQUIRE(within(p.fontSmoothing, 0.1176f, 0.0005f));
        REQUIRE(within(p.fontWeight, 0.5602f, 0.0005f));
        REQUIRE(within(p.outlineWeight, 0.4398f, 0.0005f));
    }

    SUBCASE("narrows smoothing as the effective scale increases")
    {
        const auto low = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        const auto high = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 2.0f});
        REQUIRE(high.fontSmoothing < low.fontSmoothing);
        REQUIRE(within(high.fontSmoothing, 0.0588f, 0.0005f));
    }

    SUBCASE("renders smaller on-screen text thinner")
    {
        const auto large = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        const auto small = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 13.0f, .effectiveScale = 1.0f});
        REQUIRE(small.fontWeight < large.fontWeight);
        // NOTE: Smoothing is a constant screen-space band, so it does not depend
        // on the font size.
        REQUIRE(small.fontSmoothing == approx(large.fontSmoothing));
    }

    SUBCASE("renders text drawn at a larger scale heavier")
    {
        const auto base = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        const auto scaledUp = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 2.0f});
        REQUIRE(scaledUp.fontWeight > base.fontWeight);
    }

    SUBCASE("scales the default weight by the on-screen size")
    {
        // NOTE: A 16 px font at scale 2 and a 32 px font at scale 1 are both 32 px on
        // screen, so the default weight matches. Smoothing follows the screen
        // scale, so it differs.
        const auto byFontSize = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 16.0f, .effectiveScale = 2.0f});
        const auto byScale = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        REQUIRE(byFontSize.fontWeight == approx(byScale.fontWeight));
        REQUIRE(within(byFontSize.fontSmoothing, byScale.fontSmoothing * 0.5f, 0.0005f));
    }

    SUBCASE("thickens and thins by a physical pixel amount")
    {
        const auto base = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        const auto thicker = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f, .weight = 0.5f});
        const auto thinner = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f, .weight = -0.5f});
        REQUIRE(thicker.fontWeight > base.fontWeight);
        REQUIRE(thinner.fontWeight < base.fontWeight);

        // NOTE: The same physical adjustment yields a smaller normalized step
        // at a higher DPI scale.
        const auto baseHighDPI = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 2.0f});
        const auto thickerHighDPI = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 2.0f, .weight = 0.5f});
        REQUIRE((thicker.fontWeight - base.fontWeight) >
                (thickerHighDPI.fontWeight - baseHighDPI.fontWeight));
    }

    SUBCASE("widens the outline band with outline thickness")
    {
        const auto none = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});
        const auto thin = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f, .outlineThickness = 1.0f});
        const auto thick = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f, .outlineThickness = 1.5f});

        // NOTE: The outline-to-fill boundary stays at the fill edge.
        REQUIRE(within(thin.outlineWeight, none.outlineWeight, 0.0005f));
        REQUIRE(within(thick.outlineWeight, none.outlineWeight, 0.0005f));

        // NOTE: A wider outline pushes the visible opacity edge outward, which raises
        // the public font weight.
        REQUIRE(thin.fontWeight > none.fontWeight);
        REQUIRE(thick.fontWeight > thin.fontWeight);
    }

    SUBCASE("keeps zero-valued padding transparent across a wide range")
    {
        for (const f32 fontSize : {8.0f, 13.0f, 24.0f, 32.0f, 64.0f}) {
            for (f32 scale = 0.2f; scale <= 6.0f; scale += 0.1f) {
                for (const f32 outline : {0.0f, 1.0f, 3.0f}) {
                    const auto p = computeSpriteFontSDFParameters(SpriteFontSDFDesc{
                        .fontSize = fontSize,
                        .effectiveScale = scale,
                        .outlineThickness = outline,
                    });
                    REQUIRE(p.fontSmoothing + p.fontWeight <= 1.0f + 1.0e-5f);
                }
            }
        }
    }

    SUBCASE("falls back to the reference for invalid inputs")
    {
        const auto ref = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 1.0f});

        const auto zeroScale = computeSpriteFontSDFParameters(
            SpriteFontSDFDesc{.fontSize = 32.0f, .effectiveScale = 0.0f});
        REQUIRE(zeroScale.fontSmoothing == approx(ref.fontSmoothing));
        REQUIRE(zeroScale.fontWeight == approx(ref.fontWeight));

        const auto nanSize = computeSpriteFontSDFParameters(SpriteFontSDFDesc{
            .fontSize = std::numeric_limits<f32>::quiet_NaN(),
            .effectiveScale = 1.0f,
        });
        REQUIRE(nanSize.fontSmoothing == approx(ref.fontSmoothing));
        REQUIRE(nanSize.fontWeight == approx(ref.fontWeight));
    }
}
