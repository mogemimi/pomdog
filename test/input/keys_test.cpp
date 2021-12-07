// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/keys.hpp"
#include <catch_amalgamated.hpp>
#include <type_traits>

using Pomdog::Keys;

TEST_CASE("Keys::Unknown", "[Keys]")
{
    static_assert(std::is_same<std::uint8_t, typename std::underlying_type<Keys>::type>::value,
        "Keys is std::uint8_t.");
    STATIC_REQUIRE(static_cast<std::uint8_t>(Keys::Unknown) == 0);

    STATIC_REQUIRE(Keys::Unknown != Keys::A);
    STATIC_REQUIRE(Keys::Unknown != Keys::B);
    STATIC_REQUIRE(Keys::Unknown != Keys::C);
    STATIC_REQUIRE(Keys::Unknown != Keys::D);
    STATIC_REQUIRE(Keys::Unknown != Keys::E);
    STATIC_REQUIRE(Keys::Unknown != Keys::F);
    STATIC_REQUIRE(Keys::Unknown != Keys::G);
    STATIC_REQUIRE(Keys::Unknown != Keys::H);
    STATIC_REQUIRE(Keys::Unknown != Keys::I);
    STATIC_REQUIRE(Keys::Unknown != Keys::J);
    STATIC_REQUIRE(Keys::Unknown != Keys::K);
    STATIC_REQUIRE(Keys::Unknown != Keys::L);
    STATIC_REQUIRE(Keys::Unknown != Keys::M);
    STATIC_REQUIRE(Keys::Unknown != Keys::N);
    STATIC_REQUIRE(Keys::Unknown != Keys::O);
    STATIC_REQUIRE(Keys::Unknown != Keys::P);
    STATIC_REQUIRE(Keys::Unknown != Keys::Q);
    STATIC_REQUIRE(Keys::Unknown != Keys::R);
    STATIC_REQUIRE(Keys::Unknown != Keys::S);
    STATIC_REQUIRE(Keys::Unknown != Keys::T);
    STATIC_REQUIRE(Keys::Unknown != Keys::U);
    STATIC_REQUIRE(Keys::Unknown != Keys::V);
    STATIC_REQUIRE(Keys::Unknown != Keys::W);
    STATIC_REQUIRE(Keys::Unknown != Keys::X);
    STATIC_REQUIRE(Keys::Unknown != Keys::Y);
    STATIC_REQUIRE(Keys::Unknown != Keys::Z);
}
