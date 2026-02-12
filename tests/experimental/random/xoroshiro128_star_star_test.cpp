// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <algorithm>
#include <cstdint>
#include <random>
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::random::Xoroshiro128StarStar;
using pomdog::u64;

TEST_CASE("Xoroshiro128StarStar")
{
    SUBCASE("seed == 10000")
    {
        constexpr u64 seed = 10000;
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.next() == UINT64_C(57600000));
        REQUIRE(random.next() == UINT64_C(970142572800000));
        REQUIRE(random.next() == UINT64_C(6003834865016826828));
        REQUIRE(random.next() == UINT64_C(251235842515401960));
        REQUIRE(random.next() == UINT64_C(3539055669705065779));
        REQUIRE(random.next() == UINT64_C(9824502612174526719));
        REQUIRE(random.next() == UINT64_C(10449607474323798480));
        REQUIRE(random.next() == UINT64_C(60609448284750671));
        REQUIRE(random.next() == UINT64_C(2232163366230955782));
        REQUIRE(random.next() == UINT64_C(9266923678080299712));
    }
    SUBCASE("jump, seed == 10000")
    {
        constexpr u64 seed = 10000;
        Xoroshiro128StarStar random(seed);
        random.jump();
        REQUIRE(random.next() == UINT64_C(16403608635600001139));
        REQUIRE(random.next() == UINT64_C(7887963966545594245));
        REQUIRE(random.next() == UINT64_C(12885547741085114712));
        REQUIRE(random.next() == UINT64_C(7699100600066950516));
        REQUIRE(random.next() == UINT64_C(13870152917268753319));
        REQUIRE(random.next() == UINT64_C(12057025981921697135));
        REQUIRE(random.next() == UINT64_C(1355269507196753310));
        REQUIRE(random.next() == UINT64_C(15406102805930135249));
        REQUIRE(random.next() == UINT64_C(8156156484329196414));
        REQUIRE(random.next() == UINT64_C(15846794984155927445));
    }
    SUBCASE("seed == uint64_t max")
    {
        constexpr u64 seed = UINT64_C(18446744073709551615);
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.next() == UINT64_C(18446744073709546999));
        REQUIRE(random.next() == UINT64_C(18446744073332065399));
        REQUIRE(random.next() == UINT64_C(18440435529108423799));
        REQUIRE(random.next() == UINT64_C(18445977164349173239));
        REQUIRE(random.next() == UINT64_C(7627387260232055186));
        REQUIRE(random.next() == UINT64_C(4269390498164910711));
        REQUIRE(random.next() == UINT64_C(16897953805239728575));
        REQUIRE(random.next() == UINT64_C(7746574675999783435));
        REQUIRE(random.next() == UINT64_C(2641787728794692520));
        REQUIRE(random.next() == UINT64_C(974763066663857351));
    }
    SUBCASE("uniform_int_distribution")
    {
        constexpr u64 seed = UINT64_C(10000);
        Xoroshiro128StarStar random(seed);
        std::uniform_int_distribution<int> dist(42, 99);
        for (int i = 0; i < 100; i++) {
            const auto x = dist(random);
            REQUIRE(x >= 42);
            REQUIRE(x <= 99);
        }
    }
    SUBCASE("uniform_real_distribution")
    {
        constexpr u64 seed = UINT64_C(10000);
        Xoroshiro128StarStar random(seed);
        std::uniform_real_distribution<double> dist(1.42, 3.14);
        for (int i = 0; i < 100; i++) {
            const auto x = dist(random);
            REQUIRE(x >= 1.42);
            REQUIRE(x <= 3.14);
        }
    }
    SUBCASE("preserve/reconstitute")
    {
        const u64 seedArray[] = {
            UINT64_C(0),
            UINT64_C(1),
            UINT64_C(10000),
            UINT64_C(1243456789),
            UINT64_C(18446744073709551615),
        };

        for (const auto seed : seedArray) {
            Xoroshiro128StarStar a(seed);

            {
                const auto [s0, s1] = a.preserve();
                Xoroshiro128StarStar b;
                b.reconstitute(s0, s1);
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
            }
            {
                a.jump();
                const auto [s0, s1] = a.preserve();

                Xoroshiro128StarStar b;
                b.reconstitute(s0, s1);
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
            }
            {
                a.jump();
                const auto [s0, s1] = a.preserve();

                Xoroshiro128StarStar b;
                b.reconstitute(s0, s1);
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
                REQUIRE(a.next() == b.next());
                REQUIRE(a.preserve() == b.preserve());
            }
        }
    }
}
