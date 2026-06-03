// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/gesture_tracker.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::GestureState;
using pomdog::GestureTracker;
using pomdog::Point2D;

TEST_CASE("GestureTracker")
{
    SUBCASE("reports press and release edges")
    {
        GestureTracker tracker = {};

        tracker.update(GestureState{.position = Point2D{10, 20}, .pressed = false});
        REQUIRE_FALSE(tracker.isPressed());
        REQUIRE_FALSE(tracker.pressedThisFrame());
        REQUIRE_FALSE(tracker.releasedThisFrame());

        tracker.update(GestureState{.position = Point2D{10, 20}, .pressed = true});
        REQUIRE(tracker.isPressed());
        REQUIRE(tracker.pressedThisFrame());
        REQUIRE_FALSE(tracker.releasedThisFrame());

        // NOTE: Held down: no longer a press edge.
        tracker.update(GestureState{.position = Point2D{10, 20}, .pressed = true});
        REQUIRE(tracker.isPressed());
        REQUIRE_FALSE(tracker.pressedThisFrame());
        REQUIRE_FALSE(tracker.releasedThisFrame());

        tracker.update(GestureState{.position = Point2D{10, 20}, .pressed = false});
        REQUIRE_FALSE(tracker.isPressed());
        REQUIRE_FALSE(tracker.pressedThisFrame());
        REQUIRE(tracker.releasedThisFrame());
    }

    SUBCASE("getPosition returns the latest position")
    {
        GestureTracker tracker = {};
        tracker.update(GestureState{.position = Point2D{3, 4}, .pressed = false});
        REQUIRE(tracker.getPosition() == Point2D{3, 4});
        tracker.update(GestureState{.position = Point2D{7, 9}, .pressed = true});
        REQUIRE(tracker.getPosition() == Point2D{7, 9});
    }

    SUBCASE("getDragDelta is zero on the first pressed frame")
    {
        GestureTracker tracker = {};
        tracker.update(GestureState{.position = Point2D{100, 100}, .pressed = false});
        tracker.update(GestureState{.position = Point2D{120, 130}, .pressed = true});

        // NOTE: Press edge: no delta to avoid a spurious jump.
        REQUIRE(tracker.getDragDelta() == Point2D{0, 0});
    }

    SUBCASE("getDragDelta reports movement while held")
    {
        GestureTracker tracker = {};
        tracker.update(GestureState{.position = Point2D{100, 100}, .pressed = true});
        tracker.update(GestureState{.position = Point2D{110, 90}, .pressed = true});
        REQUIRE(tracker.getDragDelta() == Point2D{10, -10});
    }

    SUBCASE("getDragDelta is zero when not pressed")
    {
        GestureTracker tracker = {};
        tracker.update(GestureState{.position = Point2D{0, 0}, .pressed = false});
        tracker.update(GestureState{.position = Point2D{50, 50}, .pressed = false});
        REQUIRE(tracker.getDragDelta() == Point2D{0, 0});
    }
}
