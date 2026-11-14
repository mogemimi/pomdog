// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/degree.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/turn.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Degree32;
using pomdog::Degree64;
using pomdog::f32;
using pomdog::f64;
using pomdog::Radian32;
using pomdog::Radian64;
using pomdog::Turn32;
using pomdog::Turn64;
namespace math = pomdog::math;

static_assert(!std::is_convertible_v<Turn32, Radian32>);
static_assert(!std::is_convertible_v<Turn32, Degree32>);
static_assert(!std::is_convertible_v<Radian32, Turn32>);
static_assert(!std::is_convertible_v<Degree32, Turn32>);

TEST_CASE("Turn32")
{
    SUBCASE("constructor")
    {
        const Turn32 t = 0.5f;
        REQUIRE_EQ(t.get(), 0.5f);
        REQUIRE_EQ(static_cast<f32>(t), 0.5f);
        REQUIRE_EQ(Turn32{}.get(), 0.0f);
    }
    SUBCASE("binary operators")
    {
        REQUIRE_EQ((Turn32{0.5f} + Turn32{0.25f}).get(), 0.75f);
        REQUIRE_EQ((Turn32{0.5f} - Turn32{0.25f}).get(), 0.25f);
        REQUIRE_EQ((Turn32{0.5f} * Turn32{0.25f}).get(), 0.125f);
        REQUIRE_EQ((Turn32{0.5f} / Turn32{0.25f}).get(), 2.0f);
        REQUIRE_EQ((Turn32{0.5f} * 3.0f).get(), 1.5f);
        REQUIRE_EQ((3.0f * Turn32{0.5f}).get(), 1.5f);
        REQUIRE_EQ((Turn32{0.5f} / 2.0f).get(), 0.25f);
        REQUIRE_EQ((-Turn32{0.5f}).get(), -0.5f);
    }
    SUBCASE("toTurn from Radian")
    {
        using pomdog::math::toTurn;
        REQUIRE_EQ(toTurn(Radian32{0.0f}), Turn32{0.0f});
        REQUIRE_EQ(toTurn(Radian32{math::PiOver2<f32>}), Turn32{0.25f});
        REQUIRE_EQ(toTurn(Radian32{math::Pi<f32>}), Turn32{0.5f});
        REQUIRE_EQ(toTurn(Radian32{-math::Pi<f32>}), Turn32{-0.5f});
        REQUIRE_EQ(toTurn(Radian32{math::TwoPi<f32>}), Turn32{1.0f});
        REQUIRE(toTurn(Radian32{1.0f}).get() == doctest::Approx(0.1591549430));

        REQUIRE_EQ(toTurn(Radian64{math::Pi<f64>}), Turn64{0.5});
        REQUIRE_EQ(toTurn(Radian64{math::TwoPi<f64>}), Turn64{1.0});
    }
    SUBCASE("toTurn from Degree")
    {
        using pomdog::math::toTurn;
        REQUIRE_EQ(toTurn(Degree32{0.0f}), Turn32{0.0f});
        REQUIRE_EQ(toTurn(Degree32{45.0f}), Turn32{0.125f});
        REQUIRE_EQ(toTurn(Degree32{90.0f}), Turn32{0.25f});
        REQUIRE_EQ(toTurn(Degree32{180.0f}), Turn32{0.5f});
        REQUIRE_EQ(toTurn(Degree32{360.0f}), Turn32{1.0f});
        REQUIRE_EQ(toTurn(Degree32{-90.0f}), Turn32{-0.25f});
        REQUIRE(toTurn(Degree32{1.0f}).get() == doctest::Approx(1.0 / 360.0));

        REQUIRE_EQ(toTurn(Degree64{90.0}), Turn64{0.25});
        REQUIRE_EQ(toTurn(Degree64{720.0}), Turn64{2.0});
    }
    SUBCASE("toRadian")
    {
        using pomdog::math::toRadian;
        REQUIRE_EQ(toRadian(Turn32{0.0f}), Radian32{0.0f});
        REQUIRE_EQ(toRadian(Turn32{0.25f}), Radian32{math::PiOver2<f32>});
        REQUIRE_EQ(toRadian(Turn32{0.5f}), Radian32{math::Pi<f32>});
        REQUIRE_EQ(toRadian(Turn32{-0.5f}), Radian32{-math::Pi<f32>});
        REQUIRE_EQ(toRadian(Turn32{1.0f}), Radian32{math::TwoPi<f32>});

        REQUIRE_EQ(toRadian(Turn64{0.5}), Radian64{math::Pi<f64>});
        REQUIRE_EQ(toRadian(Turn64{1.0}), Radian64{math::TwoPi<f64>});
    }
    SUBCASE("toDegree")
    {
        using pomdog::math::toDegree;
        REQUIRE_EQ(toDegree(Turn32{0.0f}), Degree32{0.0f});
        REQUIRE_EQ(toDegree(Turn32{0.125f}), Degree32{45.0f});
        REQUIRE_EQ(toDegree(Turn32{0.25f}), Degree32{90.0f});
        REQUIRE_EQ(toDegree(Turn32{0.5f}), Degree32{180.0f});
        REQUIRE_EQ(toDegree(Turn32{1.0f}), Degree32{360.0f});
        REQUIRE_EQ(toDegree(Turn32{-0.25f}), Degree32{-90.0f});

        REQUIRE_EQ(toDegree(Turn64{0.25}), Degree64{90.0});
        REQUIRE_EQ(toDegree(Turn64{2.0}), Degree64{720.0});
    }
    SUBCASE("normalizeSigned")
    {
        using pomdog::math::normalizeSigned;
        REQUIRE_EQ(normalizeSigned(Turn32{0.0f}), Turn32{0.0f});
        REQUIRE_EQ(normalizeSigned(Turn32{0.25f}), Turn32{0.25f});
        REQUIRE_EQ(normalizeSigned(Turn32{0.5f}), Turn32{0.5f});
        REQUIRE_EQ(normalizeSigned(Turn32{0.75f}), Turn32{-0.25f});
        REQUIRE_EQ(normalizeSigned(Turn32{1.0f}), Turn32{0.0f});
        REQUIRE_EQ(normalizeSigned(Turn32{1.25f}), Turn32{0.25f});
        REQUIRE_EQ(normalizeSigned(Turn32{-0.25f}), Turn32{-0.25f});
        REQUIRE_EQ(normalizeSigned(Turn32{-0.75f}), Turn32{0.25f});
        REQUIRE_EQ(normalizeSigned(Turn32{3.0f}), Turn32{0.0f});
        REQUIRE_EQ(normalizeSigned(Turn32{-3.25f}), Turn32{-0.25f});

        REQUIRE_EQ(normalizeSigned(Turn64{0.75}), Turn64{-0.25});
        REQUIRE_EQ(normalizeSigned(Turn64{-1.25}), Turn64{-0.25});
    }
    SUBCASE("normalizePositive")
    {
        using pomdog::math::normalizePositive;
        REQUIRE_EQ(normalizePositive(Turn32{0.0f}), Turn32{0.0f});
        REQUIRE_EQ(normalizePositive(Turn32{0.5f}), Turn32{0.5f});
        REQUIRE_EQ(normalizePositive(Turn32{1.0f}), Turn32{0.0f});
        REQUIRE_EQ(normalizePositive(Turn32{1.25f}), Turn32{0.25f});
        REQUIRE_EQ(normalizePositive(Turn32{-0.25f}), Turn32{0.75f});
        REQUIRE_EQ(normalizePositive(Turn32{-1.5f}), Turn32{0.5f});
        REQUIRE_EQ(normalizePositive(Turn32{4.75f}), Turn32{0.75f});

        REQUIRE_EQ(normalizePositive(Turn64{-0.25}), Turn64{0.75});
        REQUIRE_EQ(normalizePositive(Turn64{2.5}), Turn64{0.5});
    }
    SUBCASE("lerp")
    {
        using pomdog::math::lerp;
        REQUIRE_EQ(lerp(Turn32{0.0f}, Turn32{1.0f}, 0.0f), Turn32{0.0f});
        REQUIRE_EQ(lerp(Turn32{0.0f}, Turn32{1.0f}, 0.25f), Turn32{0.25f});
        REQUIRE_EQ(lerp(Turn32{0.0f}, Turn32{1.0f}, 1.0f), Turn32{1.0f});
    }
    SUBCASE("clamp")
    {
        using pomdog::math::clamp;
        REQUIRE_EQ(clamp(Turn32{-0.5f}, Turn32{0.0f}, Turn32{1.0f}), Turn32{0.0f});
        REQUIRE_EQ(clamp(Turn32{0.5f}, Turn32{0.0f}, Turn32{1.0f}), Turn32{0.5f});
        REQUIRE_EQ(clamp(Turn32{1.5f}, Turn32{0.0f}, Turn32{1.0f}), Turn32{1.0f});
    }
}
