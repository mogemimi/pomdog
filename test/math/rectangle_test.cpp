// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/point2d.h"
#include "pomdog/math/rectangle.h"
#include <catch_amalgamated.hpp>

using pomdog::Point2D;
using pomdog::Rectangle;

TEST_CASE("Rectangle", "[Rectangle]")
{
    SECTION("Rectangle")
    {
        using pomdog::Rectangle;
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
        REQUIRE(Point2D{320, 240} == rect.getLocation());

        REQUIRE(rect.getLeft() == 320);
        REQUIRE(320 + 640 == rect.getRight());
        REQUIRE(rect.getTop() == 240);
        REQUIRE(240 + 480 == rect.getBottom());
    }
    SECTION("Location")
    {
        using pomdog::Rectangle;
        Rectangle rect = {320, 240, 640, 480};
        REQUIRE(rect.x == 320);
        REQUIRE(rect.y == 240);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
        REQUIRE(Point2D{320, 240} == rect.getLocation());

        rect.setLocation(Point2D{-42, -24});
        REQUIRE(Point2D{-42, -24} == rect.getLocation());
        REQUIRE(rect.x == -42);
        REQUIRE(rect.y == -24);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
        REQUIRE(rect.getLeft() == -42);
        REQUIRE(-42 + 640 == rect.getRight());
        REQUIRE(rect.getTop() == -24);
        REQUIRE(-24 + 480 == rect.getBottom());
    }
    SECTION("Inflate")
    {
        using pomdog::Rectangle;
        Rectangle rect = {320, 240, 640, 480};
        rect.inflate(96, 42);
        REQUIRE(rect.x == 320 - 96);
        REQUIRE(rect.y == 240 - 42);
        REQUIRE(rect.width == 640 + (96 * 2));
        REQUIRE(rect.height == 480 + (42 * 2));
        REQUIRE(320 - 96 == rect.getLeft());
        REQUIRE(320 + 640 + 96 == rect.getRight());
        REQUIRE(240 - 42 == rect.getTop());
        REQUIRE(240 + 480 + 42 == rect.getBottom());
    }
    SECTION("Offset")
    {
        using pomdog::Rectangle;
        Rectangle rect = {320, 240, 640, 480};
        rect.offset(96, 42);
        REQUIRE(rect.x == 320 + 96);
        REQUIRE(rect.y == 240 + 42);
        REQUIRE(rect.width == 640);
        REQUIRE(rect.height == 480);
        REQUIRE(320 + 96 == rect.getLeft());
        REQUIRE(320 + 96 + 640 == rect.getRight());
        REQUIRE(240 + 42 == rect.getTop());
        REQUIRE(240 + 42 + 480 == rect.getBottom());
    }
    SECTION("Contains_Point2D")
    {
        using pomdog::Rectangle;
        Rectangle rect = {32, 24, 64, 48};
        const int left = rect.getLeft();
        const int right = rect.getRight();
        const int top = rect.getTop();
        const int bottom = rect.getBottom();
        const int centerX = rect.getCenter().x;
        const int centerY = rect.getCenter().y;

        REQUIRE(rect.contains(left, top));
        REQUIRE(rect.contains(centerX, centerY));
        REQUIRE_FALSE(rect.contains(left - 1, centerY));

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
    SECTION("Intersects_Point2D")
    {
        using pomdog::Rectangle;
        Rectangle rect = {32, 24, 64, 48};
        const int left = rect.getLeft();
        const int right = rect.getRight();
        const int top = rect.getTop();
        const int bottom = rect.getBottom();
        const int centerX = rect.getCenter().x;
        const int centerY = rect.getCenter().y;

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
    SECTION("Contains_Rectangle")
    {
        using pomdog::Rectangle;
        Rectangle rect = {32, 24, 64, 48};

        REQUIRE(rect.contains(rect));
        REQUIRE(rect.contains({32, 24, 64, 48}));
        REQUIRE_FALSE(rect.contains({32 - 1, 24, 64, 48}));
        REQUIRE_FALSE(rect.contains({32, 24 - 1, 64, 48}));
        REQUIRE_FALSE(rect.contains({32, 24, 64 + 1, 48}));
        REQUIRE_FALSE(rect.contains({32, 24, 64, 48 + 1}));
    }
    SECTION("Intersects_Rectangle")
    {
        using pomdog::Rectangle;
        Rectangle rect = {32, 24, 64, 48};

        REQUIRE(rect.intersects({32, 24, 64, 48}));
        REQUIRE(rect.intersects({32 - 1, 24, 64, 48}));
        REQUIRE(rect.intersects({32, 24 - 1, 64, 48}));
        REQUIRE(rect.intersects({32, 24, 64 + 1, 48}));
        REQUIRE(rect.intersects({32, 24, 64, 48 + 1}));
    }
}
