// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/gui/drawing_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::gui::DrawingContext;
using pomdog::gui::FontSize;

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

TEST_CASE("gui_sdf_font_parameters")
{
    DrawingContext drawingContext;
    drawingContext.reset(1280, 720, 1.0f);

    SUBCASE("returns thin, sharp values for body text")
    {
        REQUIRE(within(drawingContext.getSDFFontSmoothing(FontSize::Medium), 0.1176f, 0.0005f));
        REQUIRE(within(drawingContext.getSDFFontWeight(FontSize::Medium), 0.4970f, 0.0010f));
    }

    SUBCASE("renders larger headings heavier than body text")
    {
        REQUIRE(within(drawingContext.getSDFFontWeight(FontSize::Large), 0.5203f, 0.0010f));
        REQUIRE(drawingContext.getSDFFontWeight(FontSize::Large) >
                drawingContext.getSDFFontWeight(FontSize::Medium));

        // NOTE: Smoothing is a constant screen-space band across font sizes.
        REQUIRE(drawingContext.getSDFFontSmoothing(FontSize::Large) ==
                approx(drawingContext.getSDFFontSmoothing(FontSize::Medium)));
    }
}
