// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/Keys.hpp"
#include "catch.hpp"
#include <type_traits>

using Pomdog::Keys;

TEST_CASE("Keys::None", "[Keys]")
{
    static_assert(std::is_same<std::uint8_t, typename std::underlying_type<Keys>::type>::value,
        "Keys is std::uint8_t.");
    STATIC_REQUIRE(static_cast<std::uint8_t>(Keys::None) == 0);

    STATIC_REQUIRE(Keys::None != Keys::A);
    STATIC_REQUIRE(Keys::None != Keys::B);
    STATIC_REQUIRE(Keys::None != Keys::C);
    STATIC_REQUIRE(Keys::None != Keys::D);
    STATIC_REQUIRE(Keys::None != Keys::E);
    STATIC_REQUIRE(Keys::None != Keys::F);
    STATIC_REQUIRE(Keys::None != Keys::G);
    STATIC_REQUIRE(Keys::None != Keys::H);
    STATIC_REQUIRE(Keys::None != Keys::I);
    STATIC_REQUIRE(Keys::None != Keys::J);
    STATIC_REQUIRE(Keys::None != Keys::K);
    STATIC_REQUIRE(Keys::None != Keys::L);
    STATIC_REQUIRE(Keys::None != Keys::M);
    STATIC_REQUIRE(Keys::None != Keys::N);
    STATIC_REQUIRE(Keys::None != Keys::O);
    STATIC_REQUIRE(Keys::None != Keys::P);
    STATIC_REQUIRE(Keys::None != Keys::Q);
    STATIC_REQUIRE(Keys::None != Keys::R);
    STATIC_REQUIRE(Keys::None != Keys::S);
    STATIC_REQUIRE(Keys::None != Keys::T);
    STATIC_REQUIRE(Keys::None != Keys::U);
    STATIC_REQUIRE(Keys::None != Keys::V);
    STATIC_REQUIRE(Keys::None != Keys::W);
    STATIC_REQUIRE(Keys::None != Keys::X);
    STATIC_REQUIRE(Keys::None != Keys::Y);
    STATIC_REQUIRE(Keys::None != Keys::Z);
}
