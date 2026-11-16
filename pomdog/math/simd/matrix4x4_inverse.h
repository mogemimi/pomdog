// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/simd/f32x4.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: math::determinant(), math::adjoint(), and inverseColumns() share twelve
// 2x2 minors: s0..s5 from rows 0 and 1, and c0..c5 from rows 2 and 3.
//
//   s0 = a00*a11 - a10*a01    c0 = a20*a31 - a30*a21
//   s1 = a00*a12 - a10*a02    c1 = a20*a32 - a30*a22
//   s2 = a00*a13 - a10*a03    c2 = a20*a33 - a30*a23
//   s3 = a01*a12 - a11*a02    c3 = a21*a32 - a31*a22
//   s4 = a01*a13 - a11*a03    c4 = a21*a33 - a31*a23
//   s5 = a02*a13 - a12*a03    c5 = a22*a33 - a32*a23
//
// With r = (x, y, z, w) and k = (k0, ..., k5), let
//
//   F(r, k) = ( y*k5 - z*k4 + w*k3,
//              -x*k5 + z*k2 - w*k1,
//               x*k4 - y*k2 + w*k0,
//              -x*k3 + y*k1 - z*k0)
//
// Then the columns of adj(A) are F(row1, c), -F(row0, c), F(row3, s), and
// -F(row2, s). The determinant is the dot product of row 0 and column 0, and
// the inverse is adj(A) / det(A).
//
// With the lane swizzles yxxx, zzyy, and wwwz of a row r,
//
//   F(r, k) = (r.yxxx*(k5, k5, k4, k3) - r.zzyy*(k4, k2, k2, k1) + r.wwwz*(k3, k1, k0, k0)) * (1, -1, 1, -1)
//
// and the three vectors of minors come from the same swizzles of the two rows
// p and q that the minors use:
//
//   (k5, k5, k4, k3) = p.zzyy*q.wwwz - q.zzyy*p.wwwz
//   (k4, k2, k2, k1) = p.yxxx*q.wwwz - q.yxxx*p.wwwz
//   (k3, k1, k0, k0) = p.yxxx*q.zzyy - q.yxxx*p.zzyy
//
// NOTE: Platforms without SIMD run the same code on F32x4Scalar, which measured
// about as fast as a separate version written for scalars.

namespace pomdog::math::simd::detail {

/// SwizzledRow holds the lane swizzles yxxx, zzyy, and wwwz of a matrix row.
struct SwizzledRow final {
    F32x4 yxxx;
    F32x4 zzyy;
    F32x4 wwwz;
};

/// Returns the swizzles yxxx, zzyy, and wwwz of r.
[[nodiscard]] inline SwizzledRow
swizzleRow(F32x4 r) noexcept
{
    return SwizzledRow{
        .yxxx = swizzle<1, 0, 0, 0>(r),
        .zzyy = swizzle<2, 2, 1, 1>(r),
        .wwwz = swizzle<3, 3, 3, 2>(r),
    };
}

/// Returns the swizzles yxxx, zzyy, and wwwz of row.
[[nodiscard]] inline SwizzledRow
loadSwizzledRow(const f32 (&row)[4]) noexcept
{
    return swizzleRow(load<F32x4>(row));
}

/// Minors holds the 2x2 minors k0..k5 of two rows in the lane orders that
/// cofactors() multiplies.
struct Minors final {
    F32x4 k5543;
    F32x4 k4221;
    F32x4 k3100;
};

/// Returns a*b - c*d in each lane.
[[nodiscard]] inline F32x4
differenceOfProducts(F32x4 a, F32x4 b, F32x4 c, F32x4 d) noexcept
{
#if defined(POMDOG_ARCHITECTURE_ARM64)
    // NOTE: Both fused subtractions use the same rounded c*d. The first
    // recovers its rounding error, so equal products cancel exactly.
    const F32x4 cd = multiply(c, d);
    return subtract(subtractProduct(cd, c, d), subtractProduct(cd, a, b));
#else
    return subtract(multiply(a, b), multiply(c, d));
#endif
}

/// Returns the 2x2 minors of the rows p and q.
[[nodiscard]] inline Minors
computeMinors(const SwizzledRow& p, const SwizzledRow& q) noexcept
{
    return Minors{
        .k5543 = differenceOfProducts(p.zzyy, q.wwwz, q.zzyy, p.wwwz),
        .k4221 = differenceOfProducts(p.yxxx, q.wwwz, q.yxxx, p.wwwz),
        .k3100 = differenceOfProducts(p.yxxx, q.zzyy, q.yxxx, p.zzyy),
    };
}

/// Returns F(r, k) * (1, -1, 1, -1), a column of adj(A) before its signs.
[[nodiscard]] inline F32x4
cofactors(const SwizzledRow& r, const Minors& k) noexcept
{
    // NOTE: Round all three products before accumulation so equal terms can cancel.
    const F32x4 difference = subtract(multiply(r.yxxx, k.k5543), multiply(r.zzyy, k.k4221));
    return add(difference, multiply(r.wwwz, k.k3100));
}

/// Returns (d, -d, d, -d), where d = (t.x - t.y) + (t.z - t.w).
[[nodiscard]] inline F32x4
alternatingSums(F32x4 t) noexcept
{
    const F32x4 u = subtract(t, swizzle<1, 0, 3, 2>(t));
    return add(u, swizzle<2, 3, 0, 1>(u));
}

} // namespace pomdog::math::simd::detail

namespace pomdog::math::simd {

/// InverseColumns holds the four columns of the inverse of a 4x4 matrix.
struct InverseColumns final {
    F32x4 columns[4];
};

/// Returns the columns of the inverse of matrix, the same values that
/// math::invert() stores. The matrix must be invertible.
[[nodiscard]] inline InverseColumns
inverseColumns(const Matrix4x4& matrix) noexcept
{
    const F32x4 a0 = load<F32x4>(matrix.m[0]);
    const detail::SwizzledRow r0 = detail::swizzleRow(a0);
    const detail::SwizzledRow r1 = detail::loadSwizzledRow(matrix.m[1]);
    const detail::SwizzledRow r2 = detail::loadSwizzledRow(matrix.m[2]);
    const detail::SwizzledRow r3 = detail::loadSwizzledRow(matrix.m[3]);
    const detail::Minors s = detail::computeMinors(r0, r1);
    const detail::Minors c = detail::computeMinors(r2, r3);

    // The columns of adj(A) before the signs (1, -1, 1, -1) or (-1, 1, -1, 1).
    const F32x4 f0 = detail::cofactors(r1, c);
    const F32x4 f1 = detail::cofactors(r0, c);
    const F32x4 f2 = detail::cofactors(r3, s);
    const F32x4 f3 = detail::cofactors(r2, s);

    // NOTE: The alternating sums of row 0 times f0 give (det, -det, det, -det),
    // so the reciprocals already carry the column signs.
    const F32x4 determinants = detail::alternatingSums(multiply(a0, f0));

    [[maybe_unused]] const f32 determinant = firstLane(determinants);

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    // reciprocals = (1/det, -1/det, 1/det, -1/det)
    const F32x4 reciprocals = reciprocal(determinants);
    const F32x4 negatedReciprocals = swizzle<1, 0, 3, 2>(reciprocals);

    return InverseColumns{{
        multiply(f0, reciprocals),
        multiply(f1, negatedReciprocals),
        multiply(f2, reciprocals),
        multiply(f3, negatedReciprocals),
    }};
}

} // namespace pomdog::math::simd
