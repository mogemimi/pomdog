// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Keys;

TEST_CASE("Keys")
{
    static_assert(std::is_same_v<pomdog::u8, std::underlying_type_t<Keys>>, "Keys is u8.");

    constexpr auto toUnderlying = [](Keys key) -> std::underlying_type_t<Keys> {
        return static_cast<std::underlying_type_t<Keys>>(key);
    };

    SUBCASE("distinct values")
    {
        REQUIRE(toUnderlying(Keys::A) != toUnderlying(Keys::B));
        REQUIRE(toUnderlying(Keys::A) != toUnderlying(Keys::Space));
        REQUIRE(toUnderlying(Keys::Enter) != toUnderlying(Keys::Escape));
    }
    SUBCASE("letter keys")
    {
        REQUIRE(toUnderlying(Keys::A) >= 0);
        REQUIRE(toUnderlying(Keys::B) >= 0);
        REQUIRE(toUnderlying(Keys::C) >= 0);
        REQUIRE(toUnderlying(Keys::Z) >= 0);
    }
    SUBCASE("number keys")
    {
        REQUIRE(toUnderlying(Keys::Digit0) >= 0);
        REQUIRE(toUnderlying(Keys::Digit1) >= 0);
        REQUIRE(toUnderlying(Keys::Digit9) >= 0);
    }
    SUBCASE("function keys")
    {
        REQUIRE(toUnderlying(Keys::F1) >= 0);
        REQUIRE(toUnderlying(Keys::F2) >= 0);
        REQUIRE(toUnderlying(Keys::F12) >= 0);
    }
    SUBCASE("arrow keys")
    {
        REQUIRE(toUnderlying(Keys::UpArrow) >= 0);
        REQUIRE(toUnderlying(Keys::DownArrow) >= 0);
        REQUIRE(toUnderlying(Keys::LeftArrow) >= 0);
        REQUIRE(toUnderlying(Keys::RightArrow) >= 0);
    }
    SUBCASE("modifier keys")
    {
        REQUIRE(toUnderlying(Keys::LeftShift) >= 0);
        REQUIRE(toUnderlying(Keys::RightShift) >= 0);
        REQUIRE(toUnderlying(Keys::LeftControl) >= 0);
        REQUIRE(toUnderlying(Keys::RightControl) >= 0);
        REQUIRE(toUnderlying(Keys::LeftAlt) >= 0);
        REQUIRE(toUnderlying(Keys::RightAlt) >= 0);
    }
    SUBCASE("unknown key")
    {
        REQUIRE(toUnderlying(Keys::Unknown) == 0);
        REQUIRE(Keys::Unknown != Keys::A);
        REQUIRE(Keys::Unknown != Keys::Space);
        REQUIRE(Keys::Unknown != Keys::Enter);
    }
    SUBCASE("max key value")
    {
        constexpr pomdog::u8 maxKeyValue = 140U;
        static_assert(maxKeyValue == pomdog::MaxKeysCount);
        REQUIRE(toUnderlying(Keys::Unknown) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::BackSpace) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::A) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Z) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Digit0) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Digit1) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Digit9) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Numpad0) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Numpad1) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Numpad9) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::F1) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::F9) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::F10) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::F12) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::LeftAlt) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::RightAlt) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::LeftSuper) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::RightSuper) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Convert) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::NonConvert) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::KanaMode) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::KanjiMode) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::HangulMode) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Comma) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Equals) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Semicolon) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Plus) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Period) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Slash) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Minus) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Question) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Quote) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::DoubleQuote) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Backquote) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Backslash) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::Tilde) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::OpenBracket) < maxKeyValue);
        REQUIRE(toUnderlying(Keys::CloseBracket) < maxKeyValue);
    }
}
