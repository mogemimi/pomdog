// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u64;
using pomdog::random::Xoroshiro128StarStar;

TEST_CASE("Xoroshiro128StarStar")
{
    SUBCASE("seed == 10000")
    {
        constexpr u64 seed = 10000;
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.next() == UINT64_C(10602103511148128554));
        REQUIRE(random.next() == UINT64_C(14692389428724674096));
        REQUIRE(random.next() == UINT64_C(17749497896790207881));
        REQUIRE(random.next() == UINT64_C(7395742198572776013));
        REQUIRE(random.next() == UINT64_C(1955946137712898438));
        REQUIRE(random.next() == UINT64_C(4463778397121413054));
        REQUIRE(random.next() == UINT64_C(2849615605635769299));
        REQUIRE(random.next() == UINT64_C(1090291885625138035));
        REQUIRE(random.next() == UINT64_C(15344189064538503443));
        REQUIRE(random.next() == UINT64_C(2380324733661996857));
    }
    SUBCASE("jump, seed == 10000")
    {
        constexpr u64 seed = 10000;
        Xoroshiro128StarStar random(seed);
        random.jump();
        REQUIRE(random.next() == UINT64_C(15536446447994981503));
        REQUIRE(random.next() == UINT64_C(7483799643860914084));
        REQUIRE(random.next() == UINT64_C(5883521825371255415));
        REQUIRE(random.next() == UINT64_C(7617770229386242392));
        REQUIRE(random.next() == UINT64_C(18303536594758446952));
        REQUIRE(random.next() == UINT64_C(18029438136695013836));
        REQUIRE(random.next() == UINT64_C(273326189078482296));
        REQUIRE(random.next() == UINT64_C(6446391829907870794));
        REQUIRE(random.next() == UINT64_C(130081041958448538));
        REQUIRE(random.next() == UINT64_C(4941613668668410));
    }
    SUBCASE("seed == uint64_t max")
    {
        constexpr u64 seed = UINT64_C(18446744073709551615);
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.next() == UINT64_C(2051662207755801116));
        REQUIRE(random.next() == UINT64_C(5218942343062937440));
        REQUIRE(random.next() == UINT64_C(6348530440775344471));
        REQUIRE(random.next() == UINT64_C(7806765788545967719));
        REQUIRE(random.next() == UINT64_C(138107609852220106));
        REQUIRE(random.next() == UINT64_C(16721364177949975726));
        REQUIRE(random.next() == UINT64_C(13271580518644869806));
        REQUIRE(random.next() == UINT64_C(6422676622769718176));
        REQUIRE(random.next() == UINT64_C(11368002744435147212));
        REQUIRE(random.next() == UINT64_C(11949720846498852152));
    }
    SUBCASE("seed == 0 (previously absorbing state)")
    {
        constexpr u64 seed = 0;
        Xoroshiro128StarStar random(seed);
        REQUIRE(random.next() == UINT64_C(16053376993090331485));
        REQUIRE(random.next() == UINT64_C(7868822567099391496));
        REQUIRE(random.next() == UINT64_C(12331295923365717130));
        REQUIRE(random.next() == UINT64_C(9777274940305850302));
        REQUIRE(random.next() == UINT64_C(7260361800523965311));
        REQUIRE(random.next() == UINT64_C(8394445374354200741));
        REQUIRE(random.next() == UINT64_C(5231883115859735668));
        REQUIRE(random.next() == UINT64_C(2815982049211978224));
        REQUIRE(random.next() == UINT64_C(18378745961593794341));
        REQUIRE(random.next() == UINT64_C(13072315681693014403));
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
