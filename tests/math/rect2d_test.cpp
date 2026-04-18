// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Point2D;
using pomdog::Rect2D;

TEST_CASE("Rect2D")
{
    SUBCASE("constructor")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
    }
    SUBCASE("constructor from Point2D")
    {
        Rect2D rect = {Point2D{320, 240}, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
    }
    SUBCASE("minX")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.minX() == 320);
    }
    SUBCASE("maxX")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.maxX() == 320 + 640);
    }
    SUBCASE("minY")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.minY() == 240);
    }
    SUBCASE("maxY")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.maxY() == 240 + 480);
    }
    SUBCASE("getCenter")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.getCenter().x == 320 + 320);
        REQUIRE(rect.getCenter().y == 240 + 240);
    }
    SUBCASE("getLocation")
    {
        Rect2D rect = {320, 240, 640, 480};
        REQUIRE(rect.getLocation().x == 320);
        REQUIRE(rect.getLocation().y == 240);
    }
    SUBCASE("setLocation")
    {
        Rect2D rect = {320, 240, 640, 480};
        rect.setLocation({42, 24});
        REQUIRE(rect.getLocation().x == 42);
        REQUIRE(rect.getLocation().y == 24);
    }
    SUBCASE("inflate")
    {
        Rect2D rect = {320, 240, 640, 480};
        rect.inflate(96, 42);
        REQUIRE(rect.x == 320 - 96);
        REQUIRE(rect.y == 240 - 42);
        REQUIRE(rect.width == 640 + (96 * 2));
        REQUIRE(rect.height == 480 + (42 * 2));
    }
    SUBCASE("offset")
    {
        Rect2D rect = {320, 240, 640, 480};
        rect.offset(96, 42);
        REQUIRE(rect.x == 320 + 96);
        REQUIRE(rect.y == 240 + 42);
    }
    SUBCASE("contains Point2D true")
    {
        Rect2D rect = {32, 24, 64, 48};
        REQUIRE(rect.contains(Point2D{32, 24}));
        REQUIRE(rect.contains(Point2D{32 + 64, 24}));
        REQUIRE(rect.contains(Point2D{32, 24 + 48}));
        REQUIRE(rect.contains(Point2D{32 + 64, 24 + 48}));
        REQUIRE(rect.contains(Point2D{32 + 1, 24 + 48}));
        REQUIRE(rect.contains(Point2D{32 + 64 - 1, 24 + 48}));
        REQUIRE(rect.contains(Point2D{32 + 64, 24 + 1}));
        REQUIRE(rect.contains(Point2D{32 + 64, 24 + 48 - 1}));
        REQUIRE(rect.contains(rect.getCenter()));
    }
    SUBCASE("contains Point2D false")
    {
        Rect2D rect = {32, 24, 64, 48};
        REQUIRE_FALSE(rect.contains(Point2D{32 - 1, 24}));
        REQUIRE_FALSE(rect.contains(Point2D{32, 24 - 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64 + 1, 24}));
        REQUIRE_FALSE(rect.contains(Point2D{32, 24 + 48 + 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 - 1, 24 + 48}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64 + 1, 24 + 48}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64, 24 - 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64, 24 + 48 + 1}));
    }
    SUBCASE("contains Rect2D true")
    {
        Rect2D rect = {32, 24, 64, 48};
        REQUIRE(rect.contains(rect));
        REQUIRE(rect.contains(Rect2D{32, 24, 64, 48}));
        REQUIRE(rect.contains(Rect2D{32 + 1, 24 + 1, 64 - 2, 48 - 2}));
    }
    SUBCASE("contains Rect2D false")
    {
        Rect2D rect = {32, 24, 64, 48};
        REQUIRE_FALSE(rect.contains(Rect2D{32 - 1, 24, 64, 48}));
        REQUIRE_FALSE(rect.contains(Rect2D{32, 24 - 1, 64, 48}));
        REQUIRE_FALSE(rect.contains(Rect2D{32, 24, 64 + 1, 48}));
        REQUIRE_FALSE(rect.contains(Rect2D{32, 24, 64, 48 + 1}));
    }
    SUBCASE("intersects Rect2D true")
    {
        Rect2D rect = {32, 24, 64, 48};
        REQUIRE(rect.intersects(rect));
        REQUIRE(rect.intersects(Rect2D{32, 24, 64, 48}));
        REQUIRE(rect.intersects(Rect2D{32 + 1, 24 + 1, 64 - 2, 48 - 2}));

        // NOTE: Overlapping rectangles
        REQUIRE(rect.intersects(Rect2D{32 - 1, 24, 64, 48}));
        REQUIRE(rect.intersects(Rect2D{32, 24 - 1, 64, 48}));
        REQUIRE(rect.intersects(Rect2D{32, 24, 64 + 1, 48}));
        REQUIRE(rect.intersects(Rect2D{32, 24, 64, 48 + 1}));

        // NOTE: Adjacent rectangles (edge touching) do NOT intersect
        //       because intersects uses strict inequality (< and >)
        REQUIRE(rect.intersects(Rect2D{32 + 63, 24, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rect2D{32 - 63, 24, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rect2D{32, 24 + 47, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rect2D{32, 24 - 47, 64, 48})); // overlapping
    }
    SUBCASE("intersects Rect2D false")
    {
        Rect2D rect = {32, 24, 64, 48};

        // NOTE: Adjacent rectangles (edge touching) do NOT intersect
        REQUIRE_FALSE(rect.intersects(Rect2D{32 + 64, 24, 64, 48})); // adjacent right
        REQUIRE_FALSE(rect.intersects(Rect2D{32 - 64, 24, 64, 48})); // adjacent left
        REQUIRE_FALSE(rect.intersects(Rect2D{32, 24 + 48, 64, 48})); // adjacent bottom
        REQUIRE_FALSE(rect.intersects(Rect2D{32, 24 - 48, 64, 48})); // adjacent top
        REQUIRE_FALSE(rect.intersects(Rect2D{32 + 65, 24, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rect2D{32 - 65, 24, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rect2D{32, 24 + 49, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rect2D{32, 24 - 49, 64, 48}));
    }
    SUBCASE("contains Point2D boundary")
    {
        Rect2D rect = {32, 24, 64, 48};
        const auto left = rect.minX();
        const auto right = rect.maxX();
        const auto top = rect.minY();
        const auto bottom = rect.maxY();
        const auto centerX = rect.getCenter().x;
        const auto centerY = rect.getCenter().y;

        REQUIRE(rect.contains({left, top}));
        REQUIRE(rect.contains({left, centerY}));
        REQUIRE(rect.contains({left, bottom}));

        REQUIRE(rect.contains({right, top}));
        REQUIRE(rect.contains({right, centerY}));
        REQUIRE(rect.contains({right, bottom}));

        REQUIRE(rect.contains({left, top}));
        REQUIRE(rect.contains({centerX, top}));
        REQUIRE(rect.contains({right, top}));

        REQUIRE(rect.contains({left, bottom}));
        REQUIRE(rect.contains({centerX, bottom}));
        REQUIRE(rect.contains({right, bottom}));

        REQUIRE(rect.contains({centerX, centerY}));
        REQUIRE(rect.contains({left + 1, top + 1}));
        REQUIRE(rect.contains({left + 1, bottom - 1}));
        REQUIRE(rect.contains({right - 1, top + 1}));
        REQUIRE(rect.contains({right - 1, bottom - 1}));

        REQUIRE_FALSE(rect.contains({left - 1, centerY}));
        REQUIRE_FALSE(rect.contains({right + 1, centerY}));
        REQUIRE_FALSE(rect.contains({centerX, top - 1}));
        REQUIRE_FALSE(rect.contains({centerX, bottom + 1}));
    }
}
