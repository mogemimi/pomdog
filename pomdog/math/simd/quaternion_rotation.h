// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/simd/f32x4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::math::simd {

/// RotationTerms holds, in lanes 0 to 2, the elements of the rotation matrix
/// of a quaternion (x, y, z, w). Lane 3 holds no element.
///
///   diagonal = 1 - 2 * (yy + zz, zz + xx, xx + yy)
///   plus     = 2 * (xy + zw, yz + xw, zx + yw)
///   minus    = 2 * (xy - zw, yz - xw, zx - yw)
///
/// Row 0 of the matrix is (diagonal[0], plus[0], minus[2]), row 1 is
/// (minus[0], diagonal[1], plus[1]), and row 2 is (plus[2], minus[1],
/// diagonal[2]).
struct RotationTerms final {
    F32x4 diagonal;
    F32x4 plus;
    F32x4 minus;
};

/// Returns the elements of the rotation matrix of quaternion, each computed
/// with the same operations as the scalar formula above.
[[nodiscard]] inline RotationTerms
rotationTerms(const Quaternion& quaternion) noexcept
{
    struct Lanes final {
        f32 values[4];
    };
    const F32x4 q = load<F32x4>(std::bit_cast<Lanes>(quaternion).values);
    const F32x4 two = splat<F32x4>(2.0f);

    // NOTE: (xx, yy, zz, ww), (xy, yz, zx, ww), and (zw, xw, yw, ww).
    const F32x4 squares = multiply(q, q);
    const F32x4 products = multiply(q, swizzle<1, 2, 0, 3>(q));
    const F32x4 productsOfW = multiply(swizzle<2, 0, 1, 3>(q), swizzle<3, 3, 3, 3>(q));

    // NOTE: (yy + zz, zz + xx, xx + yy)
    const F32x4 sums = add(swizzle<1, 2, 0, 3>(squares), swizzle<2, 0, 1, 3>(squares));

    return RotationTerms{
        .diagonal = subtract(splat<F32x4>(1.0f), multiply(two, sums)),
        .plus = multiply(two, add(products, productsOfW)),
        .minus = multiply(two, subtract(products, productsOfW)),
    };
}

} // namespace pomdog::math::simd
