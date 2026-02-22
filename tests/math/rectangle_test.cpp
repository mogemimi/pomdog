// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/rectangle.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Point2D;
using pomdog::Rectangle;

TEST_CASE("Rectangle")
{
    SUBCASE("constructs")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
    }
    SUBCASE("constructs from Point2D")
    {
        Rectangle rect = {Point2D{320, 240}, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
    }
    SUBCASE("getBottom")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getBottom() == 240 + 480);
    }
    SUBCASE("getRight")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getRight() == 320 + 640);
    }
    SUBCASE("getTop")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getTop() == 240);
    }
    SUBCASE("getLeft")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getLeft() == 320);
    }
    SUBCASE("getCenter")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getCenter().x == 320 + 320);
        REQUIRE(rect.getCenter().y == 240 + 240);
    }
    SUBCASE("getLocation")
    {
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.getLocation().x == 320);
        REQUIRE(rect.getLocation().y == 240);
    }
    SUBCASE("setLocation")
    {
        Rectangle rect = {320, 240, 640, 480};
        rect.setLocation({42, 24});
        REQUIRE(rect.getLocation().x == 42);
        REQUIRE(rect.getLocation().y == 24);
    }
    SUBCASE("inflate")
    {
        Rectangle rect = {320, 240, 640, 480};
        rect.inflate(96, 42);
        REQUIRE(rect.x == 320 - 96);
        REQUIRE(rect.y == 240 - 42);
        REQUIRE(rect.width == 640 + (96 * 2));
        REQUIRE(rect.height == 480 + (42 * 2));
    }
    SUBCASE("offset")
    {
        Rectangle rect = {320, 240, 640, 480};
        rect.offset(96, 42);
        REQUIRE(rect.x == 320 + 96);
        REQUIRE(rect.y == 240 + 42);
    }
    SUBCASE("contains Point2D true")
    {
        Rectangle rect = {32, 24, 64, 48};
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
        Rectangle rect = {32, 24, 64, 48};
        REQUIRE_FALSE(rect.contains(Point2D{32 - 1, 24}));
        REQUIRE_FALSE(rect.contains(Point2D{32, 24 - 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64 + 1, 24}));
        REQUIRE_FALSE(rect.contains(Point2D{32, 24 + 48 + 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 - 1, 24 + 48}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64 + 1, 24 + 48}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64, 24 - 1}));
        REQUIRE_FALSE(rect.contains(Point2D{32 + 64, 24 + 48 + 1}));
    }
    SUBCASE("contains Rectangle true")
    {
        Rectangle rect = {32, 24, 64, 48};
        REQUIRE(rect.contains(rect));
        REQUIRE(rect.contains(Rectangle{32, 24, 64, 48}));
        REQUIRE(rect.contains(Rectangle{32 + 1, 24 + 1, 64 - 2, 48 - 2}));
    }
    SUBCASE("contains Rectangle false")
    {
        Rectangle rect = {32, 24, 64, 48};
        REQUIRE_FALSE(rect.contains(Rectangle{32 - 1, 24, 64, 48}));
        REQUIRE_FALSE(rect.contains(Rectangle{32, 24 - 1, 64, 48}));
        REQUIRE_FALSE(rect.contains(Rectangle{32, 24, 64 + 1, 48}));
        REQUIRE_FALSE(rect.contains(Rectangle{32, 24, 64, 48 + 1}));
    }
    SUBCASE("intersects Rectangle true")
    {
        Rectangle rect = {32, 24, 64, 48};
        REQUIRE(rect.intersects(rect));
        REQUIRE(rect.intersects(Rectangle{32, 24, 64, 48}));
        REQUIRE(rect.intersects(Rectangle{32 + 1, 24 + 1, 64 - 2, 48 - 2}));

        // NOTE: Overlapping rectangles
        REQUIRE(rect.intersects(Rectangle{32 - 1, 24, 64, 48}));
        REQUIRE(rect.intersects(Rectangle{32, 24 - 1, 64, 48}));
        REQUIRE(rect.intersects(Rectangle{32, 24, 64 + 1, 48}));
        REQUIRE(rect.intersects(Rectangle{32, 24, 64, 48 + 1}));

        // NOTE: Adjacent rectangles (edge touching) do NOT intersect
        //       because intersects uses strict inequality (< and >)
        REQUIRE(rect.intersects(Rectangle{32 + 63, 24, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rectangle{32 - 63, 24, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rectangle{32, 24 + 47, 64, 48})); // overlapping
        REQUIRE(rect.intersects(Rectangle{32, 24 - 47, 64, 48})); // overlapping
    }
    SUBCASE("intersects Rectangle false")
    {
        Rectangle rect = {32, 24, 64, 48};

        // NOTE: Adjacent rectangles (edge touching) do NOT intersect
        REQUIRE_FALSE(rect.intersects(Rectangle{32 + 64, 24, 64, 48})); // adjacent right
        REQUIRE_FALSE(rect.intersects(Rectangle{32 - 64, 24, 64, 48})); // adjacent left
        REQUIRE_FALSE(rect.intersects(Rectangle{32, 24 + 48, 64, 48})); // adjacent bottom
        REQUIRE_FALSE(rect.intersects(Rectangle{32, 24 - 48, 64, 48})); // adjacent top
        REQUIRE_FALSE(rect.intersects(Rectangle{32 + 65, 24, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rectangle{32 - 65, 24, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rectangle{32, 24 + 49, 64, 48}));
        REQUIRE_FALSE(rect.intersects(Rectangle{32, 24 - 49, 64, 48}));
    }
    SUBCASE("intersects Point2D true/false")
    {
        Rectangle rect = {32, 24, 64, 48};
        const auto left = rect.getLeft();
        const auto right = rect.getRight();
        const auto top = rect.getTop();
        const auto bottom = rect.getBottom();
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
