// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/backends/mouse_state.h"
#include "pomdog/input/button_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::ButtonState;
using pomdog::MouseState;

TEST_CASE("MouseState")
{
    SUBCASE("default constructor")
    {
        MouseState state = {};
        REQUIRE(state.leftButton == ButtonState::Up);
        REQUIRE(state.rightButton == ButtonState::Up);
        REQUIRE(state.middleButton == ButtonState::Up);
        REQUIRE(state.xButton1 == ButtonState::Up);
        REQUIRE(state.xButton2 == ButtonState::Up);
    }
    SUBCASE("position")
    {
        MouseState state;
        state.position.x = 100;
        state.position.y = 200;

        REQUIRE(state.position.x == 100);
        REQUIRE(state.position.y == 200);
    }
    SUBCASE("scrollWheel")
    {
        MouseState state;
        state.scrollWheel = 5;
        REQUIRE(state.scrollWheel == 5);

        state.scrollWheel = -3;
        REQUIRE(state.scrollWheel == -3);
    }
    SUBCASE("leftButton")
    {
        MouseState state;
        state.leftButton = ButtonState::Down;
        REQUIRE(state.leftButton == ButtonState::Down);

        state.leftButton = ButtonState::Up;
        REQUIRE(state.leftButton == ButtonState::Up);
    }
    SUBCASE("rightButton")
    {
        MouseState state;
        state.rightButton = ButtonState::Down;
        REQUIRE(state.rightButton == ButtonState::Down);

        state.rightButton = ButtonState::Up;
        REQUIRE(state.rightButton == ButtonState::Up);
    }
    SUBCASE("middleButton")
    {
        MouseState state;
        state.middleButton = ButtonState::Down;
        REQUIRE(state.middleButton == ButtonState::Down);

        state.middleButton = ButtonState::Up;
        REQUIRE(state.middleButton == ButtonState::Up);
    }
}
