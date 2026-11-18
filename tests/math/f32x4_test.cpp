// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/simd/f32x4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::math::simd::F32x4;
using pomdog::math::simd::F32x4Scalar;

namespace simd = pomdog::math::simd;

namespace {

template <typename V>
[[nodiscard]] V
make(f32 x, f32 y, f32 z, f32 w) noexcept
{
    const f32 values[4] = {x, y, z, w};
    return simd::load<V>(values);
}

/// Lanes holds the four lanes of a vector after simd::store().
struct Lanes final {
    f32 values[4];
};

// NOTE: Emscripten's JavaScript-based exception handling calls functions that
// may throw through JavaScript, which cannot pass a v128_t. Release builds can
// turn a reference to a vector into a v128_t argument, so this noexcept
// function stores each vector into Lanes before it reaches a function that
// throws.
template <typename V>
[[nodiscard]] Lanes
lanesOf(const V& v) noexcept
{
    Lanes lanes = {};
    simd::store(lanes.values, v);
    return lanes;
}

void requireLanes(const Lanes& lanes, f32 x, f32 y, f32 z, f32 w)
{
    REQUIRE_EQ(lanes.values[0], x);
    REQUIRE_EQ(lanes.values[1], y);
    REQUIRE_EQ(lanes.values[2], z);
    REQUIRE_EQ(lanes.values[3], w);
}

// NOTE: This is a function template instead of TEST_CASE_TEMPLATE, because GCC
// warns about the attributes of __m128 when it is an argument of a class
// template such as doctest::Types (-Wignored-attributes).
template <typename V>
void testOperations()
{
    // NOTE: Every input and result is exact in f32, so all implementations,
    // with or without fused multiply-add, return the same bits.
    SUBCASE("load and store")
    {
        requireLanes(lanesOf(make<V>(1.0f, -2.0f, 3.5f, 0.25f)), 1.0f, -2.0f, 3.5f, 0.25f);
    }
    SUBCASE("splat")
    {
        requireLanes(lanesOf(simd::splat<V>(-2.5f)), -2.5f, -2.5f, -2.5f, -2.5f);
    }
    SUBCASE("add")
    {
        requireLanes(
            lanesOf(simd::add(make<V>(1.0f, 2.0f, 3.0f, 4.0f), make<V>(10.0f, 20.0f, -30.0f, 0.5f))),
            11.0f, 22.0f, -27.0f, 4.5f);
    }
    SUBCASE("subtract")
    {
        requireLanes(
            lanesOf(simd::subtract(make<V>(1.0f, 2.0f, 3.0f, 4.0f), make<V>(10.0f, 20.0f, -30.0f, 0.5f))),
            -9.0f, -18.0f, 33.0f, 3.5f);
    }
    SUBCASE("multiply")
    {
        requireLanes(
            lanesOf(simd::multiply(make<V>(1.0f, 2.0f, 3.0f, 4.0f), make<V>(10.0f, 20.0f, -30.0f, 0.5f))),
            10.0f, 40.0f, -90.0f, 2.0f);
    }
    SUBCASE("min and max")
    {
        const V a = make<V>(1.0f, -2.0f, 3.0f, -4.0f);
        const V b = make<V>(2.0f, -3.0f, 3.0f, 5.0f);
        requireLanes(lanesOf(simd::min(a, b)), 1.0f, -3.0f, 3.0f, -4.0f);
        requireLanes(lanesOf(simd::max(a, b)), 2.0f, -2.0f, 3.0f, 5.0f);
    }
    SUBCASE("divide")
    {
        requireLanes(
            lanesOf(simd::divide(make<V>(1.0f, -2.0f, 3.0f, 4.0f), make<V>(2.0f, 4.0f, -8.0f, 0.5f))),
            0.5f, -0.5f, -0.375f, 8.0f);
    }
    SUBCASE("reciprocal")
    {
        requireLanes(
            lanesOf(simd::reciprocal(make<V>(2.0f, -4.0f, 0.5f, 8.0f))),
            0.5f, -0.25f, 2.0f, 0.125f);
    }
    SUBCASE("subtractProduct")
    {
        requireLanes(
            lanesOf(simd::subtractProduct(
                make<V>(10.0f, 20.0f, 30.0f, 40.0f),
                make<V>(1.0f, 2.0f, 3.0f, 4.0f),
                make<V>(5.0f, 6.0f, 7.0f, -8.0f))),
            5.0f, 8.0f, 9.0f, 72.0f);
    }
    SUBCASE("addProduct")
    {
        requireLanes(
            lanesOf(simd::addProduct(
                make<V>(10.0f, 20.0f, 30.0f, 40.0f),
                make<V>(1.0f, 2.0f, 3.0f, 4.0f),
                make<V>(5.0f, 6.0f, 7.0f, -8.0f))),
            15.0f, 32.0f, 51.0f, 8.0f);
    }
    SUBCASE("firstLane")
    {
        REQUIRE_EQ(simd::firstLane(make<V>(7.0f, 8.0f, 9.0f, 10.0f)), 7.0f);
    }
    SUBCASE("swizzle")
    {
        const V v = make<V>(1.0f, 2.0f, 3.0f, 4.0f);
        requireLanes(lanesOf(simd::swizzle<0, 1, 2, 3>(v)), 1.0f, 2.0f, 3.0f, 4.0f);
        requireLanes(lanesOf(simd::swizzle<3, 2, 1, 0>(v)), 4.0f, 3.0f, 2.0f, 1.0f);
        requireLanes(lanesOf(simd::swizzle<0, 0, 0, 0>(v)), 1.0f, 1.0f, 1.0f, 1.0f);
        requireLanes(lanesOf(simd::swizzle<1, 1, 1, 1>(v)), 2.0f, 2.0f, 2.0f, 2.0f);
        requireLanes(lanesOf(simd::swizzle<2, 2, 2, 2>(v)), 3.0f, 3.0f, 3.0f, 3.0f);
        requireLanes(lanesOf(simd::swizzle<3, 3, 3, 3>(v)), 4.0f, 4.0f, 4.0f, 4.0f);
        requireLanes(lanesOf(simd::swizzle<1, 0, 0, 0>(v)), 2.0f, 1.0f, 1.0f, 1.0f);
        requireLanes(lanesOf(simd::swizzle<2, 2, 1, 1>(v)), 3.0f, 3.0f, 2.0f, 2.0f);
        requireLanes(lanesOf(simd::swizzle<3, 3, 3, 2>(v)), 4.0f, 4.0f, 4.0f, 3.0f);
        requireLanes(lanesOf(simd::swizzle<1, 0, 3, 2>(v)), 2.0f, 1.0f, 4.0f, 3.0f);
        requireLanes(lanesOf(simd::swizzle<2, 3, 0, 1>(v)), 3.0f, 4.0f, 1.0f, 2.0f);
        requireLanes(lanesOf(simd::swizzle<0, 2, 1, 3>(v)), 1.0f, 3.0f, 2.0f, 4.0f);
    }
    SUBCASE("shuffle")
    {
        const V a = make<V>(1.0f, 2.0f, 3.0f, 4.0f);
        const V b = make<V>(5.0f, 6.0f, 7.0f, 8.0f);
        requireLanes(lanesOf(simd::shuffle<0, 1, 2, 3>(a, b)), 1.0f, 2.0f, 7.0f, 8.0f);
        requireLanes(lanesOf(simd::shuffle<3, 2, 1, 0>(a, b)), 4.0f, 3.0f, 6.0f, 5.0f);
        requireLanes(lanesOf(simd::shuffle<0, 0, 0, 0>(a, b)), 1.0f, 1.0f, 5.0f, 5.0f);
        requireLanes(lanesOf(simd::shuffle<3, 3, 3, 3>(a, b)), 4.0f, 4.0f, 8.0f, 8.0f);
        requireLanes(lanesOf(simd::shuffle<0, 2, 0, 2>(a, b)), 1.0f, 3.0f, 5.0f, 7.0f);
        requireLanes(lanesOf(simd::shuffle<1, 3, 1, 3>(a, b)), 2.0f, 4.0f, 6.0f, 8.0f);
        requireLanes(lanesOf(simd::shuffle<2, 1, 0, 0>(a, b)), 3.0f, 2.0f, 5.0f, 5.0f);
        requireLanes(lanesOf(simd::shuffle<1, 0, 3, 2>(b, a)), 6.0f, 5.0f, 4.0f, 3.0f);
    }
    SUBCASE("storeTransposed")
    {
        f32 rows[4][4] = {};
        simd::storeTransposed(
            rows,
            make<V>(0.0f, 1.0f, 2.0f, 3.0f),
            make<V>(4.0f, 5.0f, 6.0f, 7.0f),
            make<V>(8.0f, 9.0f, 10.0f, 11.0f),
            make<V>(12.0f, 13.0f, 14.0f, 15.0f));
        for (i32 row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 4; ++column) {
                CAPTURE(row);
                CAPTURE(column);
                POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
                REQUIRE_EQ(rows[row][column], static_cast<f32>(column * 4 + row));
                POMDOG_CLANG_UNSAFE_BUFFER_END
            }
        }
    }
}

} // namespace

TEST_CASE("F32x4Scalar")
{
    testOperations<F32x4Scalar>();
}

TEST_CASE("F32x4")
{
    testOperations<F32x4>();
}

namespace {

// NOTE: Overload resolution tells whether F32x4 is F32x4Scalar, because GCC
// warns about the attributes of __m128 in std::is_same_v (-Wignored-attributes).
[[maybe_unused]] [[nodiscard]] constexpr bool
isScalar(const F32x4Scalar*) noexcept
{
    return true;
}

[[maybe_unused]] [[nodiscard]] constexpr bool
isScalar(const void*) noexcept
{
    return false;
}

} // namespace

TEST_CASE("HasSIMD")
{
    static_assert(simd::HasSIMD == !isScalar(static_cast<const F32x4*>(nullptr)));
}
