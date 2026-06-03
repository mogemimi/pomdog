// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/application/high_dpi_settings.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::computeEffectivePixelRatio;
using pomdog::computeUnclampedPixelRatio;
using pomdog::HighDPIMode;
using pomdog::HighDPISettings;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("computeEffectivePixelRatio")
{
    SUBCASE("clamps to maxPixelRatio (used on the Emscripten canvas)")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 2.0f;

        // NOTE: A 3x browser DPR is clamped to the 2.0 cap.
        CHECK(computeEffectivePixelRatio(settings, 3.0f) == approx(2.0f));

        // NOTE: Ratios below the cap pass through unchanged.
        CHECK(computeEffectivePixelRatio(settings, 2.0f) == approx(2.0f));
        CHECK(computeEffectivePixelRatio(settings, 1.0f) == approx(1.0f));
    }

    SUBCASE("respects a larger cap")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 3.0f;
        CHECK(computeEffectivePixelRatio(settings, 3.0f) == approx(3.0f));
        CHECK(computeEffectivePixelRatio(settings, 4.0f) == approx(3.0f));
    }

    SUBCASE("Disabled always returns 1.0")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Disabled;
        settings.maxPixelRatio = 2.0f;
        CHECK(computeEffectivePixelRatio(settings, 3.0f) == approx(1.0f));
        CHECK(computeEffectivePixelRatio(settings, 1.0f) == approx(1.0f));
    }

    SUBCASE("non-positive raw ratio falls back to 1.0")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 2.0f;
        CHECK(computeEffectivePixelRatio(settings, 0.0f) == approx(1.0f));
        CHECK(computeEffectivePixelRatio(settings, -1.0f) == approx(1.0f));
    }
}

TEST_CASE("computeUnclampedPixelRatio")
{
    SUBCASE("ignores maxPixelRatio (desktop reports the unclamped ratio)")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 2.0f;

        // NOTE: On desktop the platform-reported ratio is returned unchanged
        // even when it exceeds maxPixelRatio, so pixelRatio stays consistent
        // with the actual back-buffer size.
        CHECK(computeUnclampedPixelRatio(settings, 3.0f) == approx(3.0f));
        CHECK(computeUnclampedPixelRatio(settings, 2.0f) == approx(2.0f));
        CHECK(computeUnclampedPixelRatio(settings, 1.0f) == approx(1.0f));
        CHECK(computeUnclampedPixelRatio(settings, 1.5f) == approx(1.5f));
    }

    SUBCASE("a smaller cap does not reduce the reported ratio")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 1.5f;

        // NOTE: The 1.5 "performance" preset is a no-op on desktop.
        CHECK(computeUnclampedPixelRatio(settings, 2.0f) == approx(2.0f));
    }

    SUBCASE("Disabled always returns 1.0")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Disabled;
        settings.maxPixelRatio = 2.0f;
        CHECK(computeUnclampedPixelRatio(settings, 3.0f) == approx(1.0f));
        CHECK(computeUnclampedPixelRatio(settings, 2.0f) == approx(1.0f));
    }

    SUBCASE("non-positive raw ratio falls back to 1.0")
    {
        HighDPISettings settings = {};
        settings.mode = HighDPIMode::Enabled;
        settings.maxPixelRatio = 2.0f;
        CHECK(computeUnclampedPixelRatio(settings, 0.0f) == approx(1.0f));
        CHECK(computeUnclampedPixelRatio(settings, -2.0f) == approx(1.0f));
    }
}
