// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"
#include "catch_amalgamated.hpp"
#include <algorithm>
#include <cstdint>
#include <random>

using Pomdog::Random::Xoroshiro128StarStar;

TEST_CASE("Xoroshiro128StarStar", "[Random]")
{
    SECTION("seed == 10000")
    {
        constexpr std::uint64_t seed = 10000;
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.Next() == UINT64_C(57600000));
        REQUIRE(random.Next() == UINT64_C(970142572800000));
        REQUIRE(random.Next() == UINT64_C(6003834865016826828));
        REQUIRE(random.Next() == UINT64_C(251235842515401960));
        REQUIRE(random.Next() == UINT64_C(3539055669705065779));
        REQUIRE(random.Next() == UINT64_C(9824502612174526719));
        REQUIRE(random.Next() == UINT64_C(10449607474323798480));
        REQUIRE(random.Next() == UINT64_C(60609448284750671));
        REQUIRE(random.Next() == UINT64_C(2232163366230955782));
        REQUIRE(random.Next() == UINT64_C(9266923678080299712));
    }
    SECTION("jump, seed == 10000")
    {
        constexpr std::uint64_t seed = 10000;
        Xoroshiro128StarStar random(seed);
        random.Jump();
        REQUIRE(random.Next() == UINT64_C(16403608635600001139));
        REQUIRE(random.Next() == UINT64_C(7887963966545594245));
        REQUIRE(random.Next() == UINT64_C(12885547741085114712));
        REQUIRE(random.Next() == UINT64_C(7699100600066950516));
        REQUIRE(random.Next() == UINT64_C(13870152917268753319));
        REQUIRE(random.Next() == UINT64_C(12057025981921697135));
        REQUIRE(random.Next() == UINT64_C(1355269507196753310));
        REQUIRE(random.Next() == UINT64_C(15406102805930135249));
        REQUIRE(random.Next() == UINT64_C(8156156484329196414));
        REQUIRE(random.Next() == UINT64_C(15846794984155927445));
    }
    SECTION("seed == uint64_t max")
    {
        constexpr std::uint64_t seed = UINT64_C(18446744073709551615);
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.Next() == UINT64_C(18446744073709546999));
        REQUIRE(random.Next() == UINT64_C(18446744073332065399));
        REQUIRE(random.Next() == UINT64_C(18440435529108423799));
        REQUIRE(random.Next() == UINT64_C(18445977164349173239));
        REQUIRE(random.Next() == UINT64_C(7627387260232055186));
        REQUIRE(random.Next() == UINT64_C(4269390498164910711));
        REQUIRE(random.Next() == UINT64_C(16897953805239728575));
        REQUIRE(random.Next() == UINT64_C(7746574675999783435));
        REQUIRE(random.Next() == UINT64_C(2641787728794692520));
        REQUIRE(random.Next() == UINT64_C(974763066663857351));
    }
    SECTION("uniform_int_distribution")
    {
        constexpr std::uint64_t seed = UINT64_C(10000);
        Xoroshiro128StarStar random(seed);
        std::uniform_int_distribution<int> dist(42, 99);
        for (int i = 0; i < 100; i++) {
            const auto x = dist(random);
            REQUIRE(x >= 42);
            REQUIRE(x <= 99);
        }
    }
    SECTION("uniform_real_distribution")
    {
        constexpr std::uint64_t seed = UINT64_C(10000);
        Xoroshiro128StarStar random(seed);
        std::uniform_real_distribution<double> dist(1.42, 3.14);
        for (int i = 0; i < 100; i++) {
            const auto x = dist(random);
            REQUIRE(x >= 1.42);
            REQUIRE(x <= 3.14);
        }
    }
}
