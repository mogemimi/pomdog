// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keyboard_state.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::KeyboardState;
using pomdog::Keys;
using pomdog::KeyState;

TEST_CASE("KeyboardState")
{
    SUBCASE("default constructor")
    {
        KeyboardState state;
        REQUIRE(state.isKeyUp(Keys::A));
        REQUIRE(state.isKeyUp(Keys::Space));
        REQUIRE(state.isKeyUp(Keys::Enter));
        REQUIRE_FALSE(state.isKeyDown(Keys::A));
        REQUIRE_FALSE(state.isKeyDown(Keys::Space));
        REQUIRE_FALSE(state.isKeyDown(Keys::Enter));
    }
    SUBCASE("setKey with KeyState::Down")
    {
        KeyboardState state;
        state.setKey(Keys::A, KeyState::Down);

        REQUIRE(state.isKeyDown(Keys::A));
        REQUIRE_FALSE(state.isKeyUp(Keys::A));
    }
    SUBCASE("setKey with KeyState::Up")
    {
        KeyboardState state;
        state.setKey(Keys::A, KeyState::Down);
        state.setKey(Keys::A, KeyState::Up);

        REQUIRE(state.isKeyUp(Keys::A));
        REQUIRE_FALSE(state.isKeyDown(Keys::A));
    }
    SUBCASE("multiple keys")
    {
        KeyboardState state;
        state.setKey(Keys::A, KeyState::Down);
        state.setKey(Keys::B, KeyState::Down);
        state.setKey(Keys::C, KeyState::Down);

        REQUIRE(state.isKeyDown(Keys::A));
        REQUIRE(state.isKeyDown(Keys::B));
        REQUIRE(state.isKeyDown(Keys::C));
        REQUIRE(state.isKeyUp(Keys::D));
    }
    SUBCASE("clearAllKeys")
    {
        KeyboardState state;
        state.setKey(Keys::A, KeyState::Down);
        state.setKey(Keys::B, KeyState::Down);
        state.clearAllKeys();

        REQUIRE(state.isKeyUp(Keys::A));
        REQUIRE(state.isKeyUp(Keys::B));
    }
}
