// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/easing.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace math = pomdog::math;
using pomdog::f32;
using pomdog::f64;

namespace {

auto approxF32(f32 x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(1e-5);
}

auto approxF64(f64 x) -> auto
{
    return doctest::Approx(x).epsilon(1e-6);
}

} // namespace

TEST_CASE("easing")
{
    SUBCASE("easeLinear")
    {
        REQUIRE(math::easeLinear(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeLinear(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeLinear(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeLinear(0.0) == approxF64(0.0));
        REQUIRE(math::easeLinear(1.0) == approxF64(1.0));
        REQUIRE(math::easeLinear(0.5) == approxF64(0.5));
    }

    // MARK: Quad

    SUBCASE("easeInQuad")
    {
        REQUIRE(math::easeInQuad(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInQuad(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInQuad(0.5f) == approxF32(0.25f));
        REQUIRE(math::easeInQuad(0.0) == approxF64(0.0));
        REQUIRE(math::easeInQuad(1.0) == approxF64(1.0));
        REQUIRE(math::easeInQuad(0.5) == approxF64(0.25));
    }

    SUBCASE("easeOutQuad")
    {
        REQUIRE(math::easeOutQuad(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutQuad(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutQuad(0.5f) == approxF32(0.75f));
        REQUIRE(math::easeOutQuad(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutQuad(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutQuad(0.5) == approxF64(0.75));
    }

    SUBCASE("easeInOutQuad")
    {
        REQUIRE(math::easeInOutQuad(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutQuad(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutQuad(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutQuad(0.25f) == approxF32(0.125f));
        REQUIRE(math::easeInOutQuad(0.75f) == approxF32(0.875f));
        REQUIRE(math::easeInOutQuad(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutQuad(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutQuad(0.5) == approxF64(0.5));
    }

    // MARK: Cubic

    SUBCASE("easeInCubic")
    {
        REQUIRE(math::easeInCubic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInCubic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInCubic(0.5f) == approxF32(0.125f));
        REQUIRE(math::easeInCubic(0.0) == approxF64(0.0));
        REQUIRE(math::easeInCubic(1.0) == approxF64(1.0));
        REQUIRE(math::easeInCubic(0.5) == approxF64(0.125));
    }

    SUBCASE("easeOutCubic")
    {
        REQUIRE(math::easeOutCubic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutCubic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutCubic(0.5f) == approxF32(0.875f));
        REQUIRE(math::easeOutCubic(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutCubic(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutCubic(0.5) == approxF64(0.875));
    }

    SUBCASE("easeInOutCubic")
    {
        REQUIRE(math::easeInOutCubic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutCubic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutCubic(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutCubic(0.25f) == approxF32(0.0625f));
        REQUIRE(math::easeInOutCubic(0.75f) == approxF32(0.9375f));
        REQUIRE(math::easeInOutCubic(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutCubic(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutCubic(0.5) == approxF64(0.5));
    }

    // MARK: Quart

    SUBCASE("easeInQuart")
    {
        REQUIRE(math::easeInQuart(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInQuart(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInQuart(0.5f) == approxF32(0.0625f));
        REQUIRE(math::easeInQuart(0.0) == approxF64(0.0));
        REQUIRE(math::easeInQuart(1.0) == approxF64(1.0));
        REQUIRE(math::easeInQuart(0.5) == approxF64(0.0625));
    }

    SUBCASE("easeOutQuart")
    {
        REQUIRE(math::easeOutQuart(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutQuart(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutQuart(0.5f) == approxF32(0.9375f));
        REQUIRE(math::easeOutQuart(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutQuart(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutQuart(0.5) == approxF64(0.9375));
    }

    SUBCASE("easeInOutQuart")
    {
        REQUIRE(math::easeInOutQuart(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutQuart(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutQuart(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutQuart(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutQuart(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutQuart(0.5) == approxF64(0.5));
    }

    // MARK: Quint

    SUBCASE("easeInQuint")
    {
        REQUIRE(math::easeInQuint(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInQuint(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInQuint(0.5f) == approxF32(0.03125f));
        REQUIRE(math::easeInQuint(0.0) == approxF64(0.0));
        REQUIRE(math::easeInQuint(1.0) == approxF64(1.0));
        REQUIRE(math::easeInQuint(0.5) == approxF64(0.03125));
    }

    SUBCASE("easeOutQuint")
    {
        REQUIRE(math::easeOutQuint(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutQuint(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutQuint(0.5f) == approxF32(0.96875f));
        REQUIRE(math::easeOutQuint(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutQuint(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutQuint(0.5) == approxF64(0.96875));
    }

    SUBCASE("easeInOutQuint")
    {
        REQUIRE(math::easeInOutQuint(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutQuint(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutQuint(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutQuint(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutQuint(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutQuint(0.5) == approxF64(0.5));
    }

    // MARK: Sine

    SUBCASE("easeInSine")
    {
        REQUIRE(math::easeInSine(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInSine(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInSine(0.5f) == doctest::Approx(0.29289).epsilon(1e-4));
        REQUIRE(math::easeInSine(0.0) == approxF64(0.0));
        REQUIRE(math::easeInSine(1.0) == approxF64(1.0));
        REQUIRE(math::easeInSine(0.5) == doctest::Approx(0.29289321881).epsilon(1e-8));
    }

    SUBCASE("easeOutSine")
    {
        REQUIRE(math::easeOutSine(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutSine(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutSine(0.5f) == doctest::Approx(0.70711).epsilon(1e-4));
        REQUIRE(math::easeOutSine(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutSine(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutSine(0.5) == doctest::Approx(0.70710678118).epsilon(1e-8));
    }

    SUBCASE("easeInOutSine")
    {
        REQUIRE(math::easeInOutSine(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutSine(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutSine(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutSine(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutSine(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutSine(0.5) == approxF64(0.5));
    }

    // MARK: Expo

    SUBCASE("easeInExpo")
    {
        REQUIRE(math::easeInExpo(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInExpo(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInExpo(0.0) == approxF64(0.0));
        REQUIRE(math::easeInExpo(1.0) == approxF64(1.0));
        REQUIRE(math::easeInExpo(0.1f) == doctest::Approx(0.001953125).epsilon(1e-4));
        REQUIRE(math::easeInExpo(0.1) == doctest::Approx(0.001953125).epsilon(1e-8));
    }

    SUBCASE("easeOutExpo")
    {
        REQUIRE(math::easeOutExpo(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutExpo(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutExpo(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutExpo(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutExpo(0.9f) == doctest::Approx(0.998046875).epsilon(1e-4));
        REQUIRE(math::easeOutExpo(0.9) == doctest::Approx(0.998046875).epsilon(1e-8));
    }

    SUBCASE("easeInOutExpo")
    {
        REQUIRE(math::easeInOutExpo(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutExpo(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutExpo(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutExpo(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutExpo(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutExpo(0.5) == approxF64(0.5));
    }

    // MARK: Circ

    SUBCASE("easeInCirc")
    {
        REQUIRE(math::easeInCirc(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInCirc(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInCirc(0.5f) == doctest::Approx(0.13397460).epsilon(1e-4));
        REQUIRE(math::easeInCirc(0.0) == approxF64(0.0));
        REQUIRE(math::easeInCirc(1.0) == approxF64(1.0));
        REQUIRE(math::easeInCirc(0.5) == doctest::Approx(0.13397459621).epsilon(1e-8));
    }

    SUBCASE("easeOutCirc")
    {
        REQUIRE(math::easeOutCirc(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutCirc(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutCirc(0.5f) == doctest::Approx(0.86602540).epsilon(1e-4));
        REQUIRE(math::easeOutCirc(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutCirc(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutCirc(0.5) == doctest::Approx(0.86602540378).epsilon(1e-8));
    }

    SUBCASE("easeInOutCirc")
    {
        REQUIRE(math::easeInOutCirc(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutCirc(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutCirc(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutCirc(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutCirc(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutCirc(0.5) == approxF64(0.5));
    }

    // MARK: Elastic

    SUBCASE("easeInElastic")
    {
        REQUIRE(math::easeInElastic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInElastic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInElastic(0.0) == approxF64(0.0));
        REQUIRE(math::easeInElastic(1.0) == approxF64(1.0));
    }

    SUBCASE("easeOutElastic")
    {
        REQUIRE(math::easeOutElastic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutElastic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutElastic(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutElastic(1.0) == approxF64(1.0));
    }

    SUBCASE("easeInOutElastic")
    {
        REQUIRE(math::easeInOutElastic(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutElastic(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutElastic(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutElastic(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutElastic(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutElastic(0.5) == approxF64(0.5));
    }

    // MARK: Bounce

    SUBCASE("easeInBounce")
    {
        REQUIRE(math::easeInBounce(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInBounce(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInBounce(0.0) == approxF64(0.0));
        REQUIRE(math::easeInBounce(1.0) == approxF64(1.0));
    }

    SUBCASE("easeOutBounce")
    {
        REQUIRE(math::easeOutBounce(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutBounce(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutBounce(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutBounce(1.0) == approxF64(1.0));
    }

    SUBCASE("easeInOutBounce")
    {
        REQUIRE(math::easeInOutBounce(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutBounce(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutBounce(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutBounce(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutBounce(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutBounce(0.5) == approxF64(0.5));
    }

    // MARK: Back

    SUBCASE("easeInBack")
    {
        REQUIRE(math::easeInBack(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInBack(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInBack(0.5f) == approxF32(-0.0876975f));
        REQUIRE(math::easeInBack(0.0) == approxF64(0.0));
        REQUIRE(math::easeInBack(1.0) == approxF64(1.0));
        REQUIRE(math::easeInBack(0.5) == approxF64(-0.0876975));
    }

    SUBCASE("easeOutBack")
    {
        REQUIRE(math::easeOutBack(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeOutBack(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeOutBack(0.5f) == approxF32(1.0876975f));
        REQUIRE(math::easeOutBack(0.0) == approxF64(0.0));
        REQUIRE(math::easeOutBack(1.0) == approxF64(1.0));
        REQUIRE(math::easeOutBack(0.5) == approxF64(1.0876975));
    }

    SUBCASE("easeInOutBack")
    {
        REQUIRE(math::easeInOutBack(0.0f) == approxF32(0.0f));
        REQUIRE(math::easeInOutBack(1.0f) == approxF32(1.0f));
        REQUIRE(math::easeInOutBack(0.5f) == approxF32(0.5f));
        REQUIRE(math::easeInOutBack(0.0) == approxF64(0.0));
        REQUIRE(math::easeInOutBack(1.0) == approxF64(1.0));
        REQUIRE(math::easeInOutBack(0.5) == approxF64(0.5));
    }

    // MARK: In/Out symmetry: easeOut(x) == 1 - easeIn(1 - x) for non-overshoot functions

    SUBCASE("easeOut is complement of easeIn (Quad)")
    {
        const auto x = 0.3f;
        REQUIRE(math::easeOutQuad(x) == approxF32(1.0f - math::easeInQuad(1.0f - x)));
    }

    SUBCASE("easeOut is complement of easeIn (Cubic)")
    {
        const auto x = 0.3f;
        REQUIRE(math::easeOutCubic(x) == approxF32(1.0f - math::easeInCubic(1.0f - x)));
    }

    SUBCASE("easeOut is complement of easeIn (Sine)")
    {
        const auto x = 0.3f;
        REQUIRE(math::easeOutSine(x) == approxF32(1.0f - math::easeInSine(1.0f - x)));
    }

    SUBCASE("easeOut is complement of easeIn (Circ)")
    {
        const auto x = 0.3f;
        REQUIRE(math::easeOutCirc(x) == approxF32(1.0f - math::easeInCirc(1.0f - x)));
    }

    SUBCASE("easeOut is complement of easeIn (Bounce)")
    {
        const auto x = 0.3f;
        REQUIRE(math::easeOutBounce(x) == approxF32(1.0f - math::easeInBounce(1.0f - x)));
    }
}
