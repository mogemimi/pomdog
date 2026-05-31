// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/pixel_ratio_conversion.h"
#include "pomdog/math/point2d.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::Point2D;
using pomdog::Rect2D;
using pomdog::Vector2;
namespace math = pomdog::math;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("pixel_ratio_conversion")
{
    SUBCASE("toPhysicalPixels for Point2D scales each component")
    {
        const auto logical = Point2D{100, 50};
        REQUIRE(math::toPhysicalPixels(logical, 1.0f) == Point2D{100, 50});
        REQUIRE(math::toPhysicalPixels(logical, 2.0f) == Point2D{200, 100});
        REQUIRE(math::toPhysicalPixels(logical, 1.5f) == Point2D{150, 75});
    }

    SUBCASE("toLogicalPixels for Point2D is the inverse of toPhysicalPixels")
    {
        const auto physical = Point2D{200, 100};
        REQUIRE(math::toLogicalPixels(physical, 2.0f) == Point2D{100, 50});
        REQUIRE(math::toLogicalPixels(physical, 1.0f) == Point2D{200, 100});
    }

    SUBCASE("Point2D conversion rounds to nearest integer")
    {
        const auto logical = Point2D{3, 5};
        const auto physical = math::toPhysicalPixels(logical, 1.5f);
        REQUIRE((physical.x == 4 || physical.x == 5));
        REQUIRE((physical.y == 7 || physical.y == 8));
    }

    SUBCASE("toPhysicalPixels for Vector2 does no rounding")
    {
        const auto logical = Vector2{100.0f, 50.0f};
        const auto physical = math::toPhysicalPixels(logical, 1.5f);
        REQUIRE(physical.x == approx(150.0f));
        REQUIRE(physical.y == approx(75.0f));
    }

    SUBCASE("toLogicalPixels for Vector2 does no rounding")
    {
        const auto physical = Vector2{300.0f, 150.0f};
        const auto logical = math::toLogicalPixels(physical, 3.0f);
        REQUIRE(logical.x == approx(100.0f));
        REQUIRE(logical.y == approx(50.0f));
    }

    SUBCASE("Vector2 round-trip preserves value")
    {
        const auto original = Vector2{123.5f, 67.25f};
        const f32 ratio = 2.0f;
        const auto roundTrip = math::toLogicalPixels(
            math::toPhysicalPixels(original, ratio),
            ratio);
        REQUIRE(roundTrip.x == approx(original.x));
        REQUIRE(roundTrip.y == approx(original.y));
    }

    SUBCASE("Rect2D conversion scales x, y, width, and height")
    {
        const auto logical = Rect2D{10, 20, 100, 50};
        const auto physical = math::toPhysicalPixels(logical, 2.0f);
        REQUIRE(physical.x == 20);
        REQUIRE(physical.y == 40);
        REQUIRE(physical.width == 200);
        REQUIRE(physical.height == 100);
    }

    SUBCASE("Rect2D toLogicalPixels is the inverse of toPhysicalPixels")
    {
        const auto physical = Rect2D{20, 40, 200, 100};
        const auto logical = math::toLogicalPixels(physical, 2.0f);
        REQUIRE(logical.x == 10);
        REQUIRE(logical.y == 20);
        REQUIRE(logical.width == 100);
        REQUIRE(logical.height == 50);
    }

    SUBCASE("pixelRatio of 1.0 is the identity")
    {
        REQUIRE(math::toPhysicalPixels(Point2D{7, 13}, 1.0f) == Point2D{7, 13});
        REQUIRE(math::toLogicalPixels(Point2D{7, 13}, 1.0f) == Point2D{7, 13});
        REQUIRE(math::toPhysicalPixels(Vector2{1.5f, 2.5f}, 1.0f).x == approx(1.5f));
        REQUIRE(math::toLogicalPixels(Rect2D{1, 2, 3, 4}, 1.0f) == Rect2D{1, 2, 3, 4});
    }
}
