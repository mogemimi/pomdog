// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Input/KeyboardState.hpp>
#include <Pomdog/Input/Keys.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::KeyboardState;
using Pomdog::Keys;
using Pomdog::KeyState;

TEST(KeyboardState, KeysToInteger)
{
    using std::uint32_t;
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::None));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::BackSpace));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::A));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Z));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Alpha0));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Alpha1));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Alpha9));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Keypad0));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Keypad1));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Keypad9));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::F1));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::F9));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::F10));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::F12));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::LeftAlt));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::RightAlt));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::LeftApple));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::RightApple));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::LeftWindows));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::RightWindows));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::IMEConvert));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::IMENoConvert));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::KanaMode));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::KanjiMode));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::HangulMode));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Comma));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Equals));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Semicolon));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Plus));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Period));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Slash));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Minus));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Question));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Quote));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::DoubleQuote));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::AccentGrave));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Backslash));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::Tilde));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::OpenBracket));
    EXPECT_GT(128U, static_cast<uint32_t>(Keys::CloseBracket));
}
