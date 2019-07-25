// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "catch.hpp"

using Pomdog::KeyboardState;
using Pomdog::Keys;
using Pomdog::KeyState;

TEST_CASE("KeysToInteger", "[KeyboardState]")
{
    using std::uint32_t;
    REQUIRE(static_cast<uint32_t>(Keys::None) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::BackSpace) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::A) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Z) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha0) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha1) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Alpha9) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad0) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad1) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Keypad9) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::F1) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::F9) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::F10) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::F12) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::LeftAlt) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::RightAlt) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::LeftApple) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::RightApple) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::LeftWindows) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::RightWindows) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::IMEConvert) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::IMENoConvert) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::KanaMode) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::KanjiMode) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::HangulMode) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Comma) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Equals) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Semicolon) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Plus) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Period) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Slash) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Minus) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Question) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Quote) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::DoubleQuote) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::AccentGrave) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Backslash) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::Tilde) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::OpenBracket) < 128U);
    REQUIRE(static_cast<uint32_t>(Keys::CloseBracket) < 128U);
}
