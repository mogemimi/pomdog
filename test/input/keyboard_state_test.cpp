// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/keyboard_state.hpp"
#include "pomdog/input/keys.hpp"
#include <catch_amalgamated.hpp>

using pomdog::KeyboardState;
using pomdog::Keys;
using pomdog::KeyState;

TEST_CASE("KeysToInteger", "[KeyboardState]")
{
    using std::uint32_t;
    REQUIRE(static_cast<uint32_t>(Keys::Unknown) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::BackSpace) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::A) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Z) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha0) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha1) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha9) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad0) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad1) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad9) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::F1) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::F9) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::F10) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::F12) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::LeftAlt) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::RightAlt) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::LeftSuper) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::RightSuper) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::IMEConvert) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::IMENonConvert) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::KanaMode) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::KanjiMode) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::HangulMode) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Comma) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Equals) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Semicolon) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Plus) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Period) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Slash) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Minus) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Question) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Quote) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::DoubleQuote) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::BackQuate) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Backslash) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::Tilde) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::OpenBracket) < 142U);
    REQUIRE(static_cast<uint32_t>(Keys::CloseBracket) < 142U);
}
