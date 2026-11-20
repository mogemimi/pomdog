// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x3.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/matrix2x2.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/simd/quaternion_rotation.h"
#include "pomdog/math/trigonometry.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Matrix3x3::Matrix3x3() noexcept = default;

Matrix3x3::Matrix3x3(
    f32 m00, f32 m01, f32 m02,
    f32 m10, f32 m11, f32 m12,
    f32 m20, f32 m21, f32 m22) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other) noexcept
{
    *this = math::multiply(*this, other);
    return *this;
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[0][2] += other.m[0][2];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[1][2] += other.m[1][2];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    m[2][2] += other.m[2][2];
    return *this;
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[0][2] -= other.m[0][2];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[1][2] -= other.m[1][2];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    m[2][2] -= other.m[2][2];
    return *this;
}

Matrix3x3& Matrix3x3::operator*=(f32 factor) noexcept
{
    m[0][0] *= factor;
    m[0][1] *= factor;
    m[0][2] *= factor;
    m[1][0] *= factor;
    m[1][1] *= factor;
    m[1][2] *= factor;
    m[2][0] *= factor;
    m[2][1] *= factor;
    m[2][2] *= factor;
    return *this;
}

Matrix3x3& Matrix3x3::operator/=(f32 factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[0][2] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    m[1][2] *= inverseDivider;
    m[2][0] *= inverseDivider;
    m[2][1] *= inverseDivider;
    m[2][2] *= inverseDivider;
    return *this;
}

Matrix3x3 Matrix3x3::operator+() const noexcept
{
    return *this;
}

Matrix3x3 Matrix3x3::operator-() const noexcept
{
    return math::multiply(*this, -1.0f);
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3& other) const noexcept
{
    return Matrix3x3{
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[0][2] + other.m[0][2],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[1][2] + other.m[1][2],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1],
        m[2][2] + other.m[2][2],
    };
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3& other) const noexcept
{
    return Matrix3x3{
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[0][2] - other.m[0][2],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[1][2] - other.m[1][2],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1],
        m[2][2] - other.m[2][2],
    };
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const noexcept
{
    return math::multiply(*this, other);
}

Matrix3x3 Matrix3x3::operator*(f32 factor) const noexcept
{
    return math::multiply(*this, factor);
}

Matrix3x3 Matrix3x3::operator/(f32 factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    return Matrix3x3{
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[0][2] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[1][2] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider,
        m[2][2] * inverseDivider,
    };
}

bool Matrix3x3::operator==(const Matrix3x3& other) const noexcept
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[0][2] == other.m[0][2] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] &&
           m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] &&
           m[2][2] == other.m[2][2];
}

bool Matrix3x3::operator!=(const Matrix3x3& other) const noexcept
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[0][2] != other.m[0][2] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] ||
           m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] ||
           m[2][2] != other.m[2][2];
}

f32 Matrix3x3::operator()(i32 row, i32 column) const noexcept
{
    static_assert(std::is_same_v<decltype(m), f32[3][3]>);
    POMDOG_ASSERT(row >= 0);
    POMDOG_ASSERT(column >= 0);
    POMDOG_ASSERT(row < 3);
    POMDOG_ASSERT(column < 3);
    return m[row][column];
}

f32& Matrix3x3::operator()(i32 row, i32 column) noexcept
{
    static_assert(std::is_same_v<decltype(m), f32[3][3]>);
    POMDOG_ASSERT(row >= 0);
    POMDOG_ASSERT(column >= 0);
    POMDOG_ASSERT(row < 3);
    POMDOG_ASSERT(column < 3);
    return m[row][column];
}

void Matrix3x3::setScale(const Vector3& scale) noexcept
{
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
}

Vector3 Matrix3x3::getScale() const noexcept
{
    return Vector3{m[0][0], m[1][1], m[2][2]};
}

Matrix3x3
Matrix3x3::createTranslation(const Vector2& position) noexcept
{
    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        position.x, position.y, 1.0f};
}

Matrix3x3
Matrix3x3::createScale(f32 scale) noexcept
{
    return Matrix3x3{
        scale, 0.0f, 0.0f,
        0.0f, scale, 0.0f,
        0.0f, 0.0f, scale};
}

Matrix3x3
Matrix3x3::createScale(const Vector3& scale) noexcept
{
    return Matrix3x3{
        scale.x, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f,
        0.0f, 0.0f, scale.z};
}

Matrix3x3
Matrix3x3::createRotationX(const Radian32& angle)
{
    const auto sinAngle = std::sin(angle.get());
    const auto cosAngle = std::cos(angle.get());

    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle,
        0.0f, -sinAngle, cosAngle};
}

Matrix3x3
Matrix3x3::createRotationX(const Turn32& angle) noexcept
{
    // NOTE: sincospi takes half turns.
    const auto [sinAngle, cosAngle] = math::sincospi(2.0f * angle.get());

    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle,
        0.0f, -sinAngle, cosAngle};
}

Matrix3x3
Matrix3x3::createRotationY(const Radian32& angle)
{
    const auto sinAngle = std::sin(angle.get());
    const auto cosAngle = std::cos(angle.get());

    return Matrix3x3{
        cosAngle, 0.0f, -sinAngle,
        0.0f, 1.0f, 0.0f,
        sinAngle, 0.0f, cosAngle};
}

Matrix3x3
Matrix3x3::createRotationY(const Turn32& angle) noexcept
{
    // NOTE: sincospi takes half turns.
    const auto [sinAngle, cosAngle] = math::sincospi(2.0f * angle.get());

    return Matrix3x3{
        cosAngle, 0.0f, -sinAngle,
        0.0f, 1.0f, 0.0f,
        sinAngle, 0.0f, cosAngle};
}

Matrix3x3
Matrix3x3::createRotationZ(const Radian32& angle)
{
    const auto sinAngle = std::sin(angle.get());
    const auto cosAngle = std::cos(angle.get());

    return Matrix3x3{
        cosAngle, sinAngle, 0.0f,
        -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 1.0f};
}

Matrix3x3
Matrix3x3::createRotationZ(const Turn32& angle) noexcept
{
    // NOTE: sincospi takes half turns.
    const auto [sinAngle, cosAngle] = math::sincospi(2.0f * angle.get());

    return Matrix3x3{
        cosAngle, sinAngle, 0.0f,
        -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 1.0f};
}

Matrix3x3
Matrix3x3::createFromQuaternion(const Quaternion& quaternion)
{
    namespace simd = math::simd;

    // NOTE: Copying the elements one by one measured faster than picking them
    // with shuffles, which need extra stores for nine elements.
    const auto terms = simd::rotationTerms(quaternion);
    f32 d[4];
    f32 p[4];
    f32 s[4];
    simd::store(d, terms.diagonal);
    simd::store(p, terms.plus);
    simd::store(s, terms.minus);

    return Matrix3x3{
        d[0], p[0], s[2],
        s[0], d[1], p[1],
        p[2], s[1], d[2]};
}

namespace {

/// Returns the matrix that rotates around the normalized axis by an angle,
/// given the sine and cosine of the angle.
[[nodiscard]] Matrix3x3
createFromAxisSinCos(const Vector3& axis, f32 sinAngle, f32 cosAngle) noexcept
{
    const auto t = 1.0f - cosAngle;
    const auto xx = axis.x * axis.x;
    const auto yy = axis.y * axis.y;
    const auto zz = axis.z * axis.z;
    const auto xy = axis.x * axis.y;
    const auto xz = axis.x * axis.z;
    const auto yz = axis.y * axis.z;

    return Matrix3x3{
        t * xx + cosAngle,
        t * xy + sinAngle * axis.z,
        t * xz - sinAngle * axis.y,
        t * xy - sinAngle * axis.z,
        t * yy + cosAngle,
        t * yz + sinAngle * axis.x,
        t * xz + sinAngle * axis.y,
        t * yz - sinAngle * axis.x,
        t * zz + cosAngle,
    };
}

} // namespace

Matrix3x3
Matrix3x3::createFromAxisAngle(const Vector3& axis, const Radian32& angle)
{
    const auto sinAngle = std::sin(angle.get());
    const auto cosAngle = std::cos(angle.get());
    return createFromAxisSinCos(axis, sinAngle, cosAngle);
}

Matrix3x3
Matrix3x3::createFromAxisAngle(const Vector3& axis, const Turn32& angle) noexcept
{
    // NOTE: sincospi takes half turns.
    const auto [sinAngle, cosAngle] = math::sincospi(2.0f * angle.get());
    return createFromAxisSinCos(axis, sinAngle, cosAngle);
}

const f32* Matrix3x3::data() const noexcept
{
    return m[0];
}

f32* Matrix3x3::data() noexcept
{
    return m[0];
}

[[nodiscard]] Matrix3x3
operator*(f32 factor, const Matrix3x3& matrix) noexcept
{
    return math::multiply(matrix, factor);
}

Matrix3x3 Matrix3x3::createIdentity() noexcept
{
    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] f32
determinant(const Matrix3x3& matrix) noexcept
{
    // NOTE:
    // | m00 m01 m02 | determinant =
    // | m10 m11 m12 |   (m00 * m11 * m22) +(m10 * m21 * m02) +(m20 *01 * m12)
    // | m20 m21 m22 |  -(m02 * m11 * m20) -(m12 * m21 * m00) -(m22 *01 * m10)
    //
    //   cofactor00    cofactor01     cofactor02
    //    |m11 m12|     |m10 m12|      |m10 m11|
    // m00|m21 m22|  m01|m20 m22|   m02|m20 m21|

    const auto& m = matrix.m;

    const auto cofactor00 = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
    const auto cofactor10 = m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]);
    const auto cofactor20 = m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    return cofactor00 + cofactor10 + cofactor20;
}

[[nodiscard]] Matrix2x2
minor2x2(const Matrix3x3& matrix, i32 row, i32 column)
{
    static_assert(std::is_same_v<decltype(matrix.m), f32[3][3]>);
    constexpr i32 rowSize = 3;
    constexpr i32 columnSize = 3;
    POMDOG_ASSERT(row >= 0);
    POMDOG_ASSERT(column >= 0);
    POMDOG_ASSERT(row < rowSize);
    POMDOG_ASSERT(column < columnSize);

    // NOTE:
    //   Matrix3x3 mat                     Matrix2x2 minor
    //     c0  c1  c2
    // r0 |11, 12, 13|                       |11, 13, x|
    // r1 |21, 22, 23| Minor2x2(mat, r2, c1) |21, 23. x|
    // r2 |31, 32, 33| --------------------> | x,  x, x|

    Matrix2x2 minorMatrix = {};
    for (i32 i = 0, s = 0; i < rowSize; i++) {
        if (row == i) {
            continue;
        }

        for (i32 j = 0, t = 0; j < columnSize; j++) {
            if (column == j) {
                continue;
            }

            minorMatrix.m[s][t] = matrix.m[i][j];
            t++;
        }
        s++;
    }
    return minorMatrix;
}

[[nodiscard]] Matrix3x3
multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) noexcept
{
    return Matrix3x3{
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] + matrix1.m[0][2] * matrix2.m[2][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] + matrix1.m[0][2] * matrix2.m[2][1],
        matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] + matrix1.m[0][2] * matrix2.m[2][2],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] + matrix1.m[1][2] * matrix2.m[2][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] + matrix1.m[1][2] * matrix2.m[2][1],
        matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] + matrix1.m[1][2] * matrix2.m[2][2],
        matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix1.m[2][2] * matrix2.m[2][0],
        matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix1.m[2][2] * matrix2.m[2][1],
        matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] + matrix1.m[2][2] * matrix2.m[2][2],
    };
}

[[nodiscard]] Matrix3x3
multiply(const Matrix3x3& matrix1, f32 factor) noexcept
{
    return Matrix3x3{
        matrix1.m[0][0] * factor,
        matrix1.m[0][1] * factor,
        matrix1.m[0][2] * factor,
        matrix1.m[1][0] * factor,
        matrix1.m[1][1] * factor,
        matrix1.m[1][2] * factor,
        matrix1.m[2][0] * factor,
        matrix1.m[2][1] * factor,
        matrix1.m[2][2] * factor,
    };
}

[[nodiscard]] Matrix3x3
adjoint(const Matrix3x3& matrix)
{
    // NOTE: The columns of adj(A) are the cross products r1 x r2, r2 x r0, and
    // r0 x r1 of the rows r0, r1, and r2 of A.
    const auto& a = matrix.m;
    return Matrix3x3{
        a[1][1] * a[2][2] - a[1][2] * a[2][1],
        a[2][1] * a[0][2] - a[2][2] * a[0][1],
        a[0][1] * a[1][2] - a[0][2] * a[1][1],
        a[1][2] * a[2][0] - a[1][0] * a[2][2],
        a[2][2] * a[0][0] - a[2][0] * a[0][2],
        a[0][2] * a[1][0] - a[0][0] * a[1][2],
        a[1][0] * a[2][1] - a[1][1] * a[2][0],
        a[2][0] * a[0][1] - a[2][1] * a[0][0],
        a[0][0] * a[1][1] - a[0][1] * a[1][0],
    };
}

[[nodiscard]] Matrix3x3
transpose(const Matrix3x3& matrix) noexcept
{
    return Matrix3x3{
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]};
}

[[nodiscard]] Matrix3x3
invert(const Matrix3x3& matrix)
{
    // NOTE: This computes adj(A) as adjoint() does, instead of calling it,
    // because MSVC does not inline adjoint() here and copying its result
    // through memory makes invert() slower.
    const auto& a = matrix.m;

    // The first column of adj(A), which the determinant shares.
    const f32 c0 = a[1][1] * a[2][2] - a[1][2] * a[2][1];
    const f32 c1 = a[1][2] * a[2][0] - a[1][0] * a[2][2];
    const f32 c2 = a[1][0] * a[2][1] - a[1][1] * a[2][0];

    const f32 determinant = a[0][0] * c0 + a[0][1] * c1 + a[0][2] * c2;

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    const f32 inverseDet = 1.0f / determinant;

    return Matrix3x3{
        c0 * inverseDet,
        (a[2][1] * a[0][2] - a[2][2] * a[0][1]) * inverseDet,
        (a[0][1] * a[1][2] - a[0][2] * a[1][1]) * inverseDet,
        c1 * inverseDet,
        (a[2][2] * a[0][0] - a[2][0] * a[0][2]) * inverseDet,
        (a[0][2] * a[1][0] - a[0][0] * a[1][2]) * inverseDet,
        c2 * inverseDet,
        (a[2][0] * a[0][1] - a[2][1] * a[0][0]) * inverseDet,
        (a[0][0] * a[1][1] - a[0][1] * a[1][0]) * inverseDet,
    };
}

[[nodiscard]] Matrix3x3
lerp(const Matrix3x3& source1, const Matrix3x3& source2, f32 amount) noexcept
{
    return Matrix3x3{
        math::lerp(source1.m[0][0], source2.m[0][0], amount),
        math::lerp(source1.m[0][1], source2.m[0][1], amount),
        math::lerp(source1.m[0][2], source2.m[0][2], amount),
        math::lerp(source1.m[1][0], source2.m[1][0], amount),
        math::lerp(source1.m[1][1], source2.m[1][1], amount),
        math::lerp(source1.m[1][2], source2.m[1][2], amount),
        math::lerp(source1.m[2][0], source2.m[2][0], amount),
        math::lerp(source1.m[2][1], source2.m[2][1], amount),
        math::lerp(source1.m[2][2], source2.m[2][2], amount),
    };
}

} // namespace pomdog::math
