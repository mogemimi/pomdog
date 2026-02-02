// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/mouse_state.h"

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
        REQUIRE(state.leftButton == ButtonState::Released);
        REQUIRE(state.rightButton == ButtonState::Released);
        REQUIRE(state.middleButton == ButtonState::Released);
        REQUIRE(state.xButton1 == ButtonState::Released);
        REQUIRE(state.xButton2 == ButtonState::Released);
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
        state.leftButton = ButtonState::Pressed;
        REQUIRE(state.leftButton == ButtonState::Pressed);

        state.leftButton = ButtonState::Released;
        REQUIRE(state.leftButton == ButtonState::Released);
    }
    SUBCASE("rightButton")
    {
        MouseState state;
        state.rightButton = ButtonState::Pressed;
        REQUIRE(state.rightButton == ButtonState::Pressed);

        state.rightButton = ButtonState::Released;
        REQUIRE(state.rightButton == ButtonState::Released);
    }
    SUBCASE("middleButton")
    {
        MouseState state;
        state.middleButton = ButtonState::Pressed;
        REQUIRE(state.middleButton == ButtonState::Pressed);

        state.middleButton = ButtonState::Released;
        REQUIRE(state.middleButton == ButtonState::Released);
    }
}
