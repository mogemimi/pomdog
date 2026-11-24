// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/simd/f32x4.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: These functions compute Vector3 operations in lanes 0 to 2 of F32x4,
// with the operations of the scalar functions of Vector3 in the same order, so
// the results match those functions bit for bit when the scalar code is not
// compiled with fused multiply-add. Lane 3 holds no element.

namespace pomdog::math::simd {

/// Returns a value whose lanes 0 to 2 are the x, y, and z of v.
[[nodiscard]] inline F32x4
loadVector3(const Vector3& v) noexcept
{
    // NOTE: Builds (x, z, y, y) and then swaps lanes 1 and 2, so that MSVC
    // does not merge the loads of x and y into one 8-byte load. When the caller
    // has just written v with 4-byte stores, as it does for a temporary, the
    // wider load cannot take its data from those stores and waits for them.
    // With the loads merged, createLookAtLH on temporaries ran 2.7 times slower
    // than the scalar code on MSVC.
    const F32x4 xz = shuffle<0, 0, 0, 0>(splat<F32x4>(v.x), splat<F32x4>(v.z));
    return swizzle<0, 2, 1, 3>(shuffle<0, 2, 0, 0>(xz, splat<F32x4>(v.y)));
}

/// Returns the Vector3 in lanes 0 to 2 of v.
[[nodiscard]] inline Vector3
storeVector3(F32x4 v) noexcept
{
    f32 lanes[4];
    store(lanes, v);
    return Vector3{lanes[0], lanes[1], lanes[2]};
}

/// Returns math::cross of the vectors in lanes 0 to 2 of a and b.
[[nodiscard]] inline F32x4
cross3(F32x4 a, F32x4 b) noexcept
{
    return subtract(
        multiply(swizzle<1, 2, 0, 3>(a), swizzle<2, 0, 1, 3>(b)),
        multiply(swizzle<2, 0, 1, 3>(a), swizzle<1, 2, 0, 3>(b)));
}

/// Returns math::dot of the vectors in lanes 0 to 2 of a and b.
[[nodiscard]] inline f32
dot3(F32x4 a, F32x4 b) noexcept
{
    const F32x4 m = multiply(a, b);
    return firstLane(add(add(m, swizzle<1, 1, 1, 1>(m)), swizzle<2, 2, 2, 2>(m)));
}

/// Returns math::normalize of the vector in lanes 0 to 2 of v.
[[nodiscard]] inline F32x4
normalize3(F32x4 v) noexcept
{
    const f32 length = std::sqrt(dot3(v, v));
    if (length > std::numeric_limits<f32>::epsilon()) {
        POMDOG_ASSERT(std::fpclassify(length) != FP_ZERO);
        POMDOG_ASSERT(std::fpclassify(length) != FP_NAN);
        POMDOG_ASSERT(std::fpclassify(length) != FP_INFINITE);
        return multiply(v, splat<F32x4>(1.0f / length));
    }
    return v;
}

} // namespace pomdog::math::simd
