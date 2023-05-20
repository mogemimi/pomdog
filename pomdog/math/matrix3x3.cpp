// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x3.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
#include "pomdog/math/matrix2x2.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
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
    float m00, float m01, float m02,
    float m10, float m11, float m12,
    float m20, float m21, float m22) noexcept
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
    *this = math::Multiply(*this, other);
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

Matrix3x3& Matrix3x3::operator*=(float factor) noexcept
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

Matrix3x3& Matrix3x3::operator/=(float factor) noexcept
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
    return math::Multiply(*this, -1.0f);
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
    return math::Multiply(*this, other);
}

Matrix3x3 Matrix3x3::operator*(float factor) const noexcept
{
    return math::Multiply(*this, factor);
}

Matrix3x3 Matrix3x3::operator/(float factor) const noexcept
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

float& Matrix3x3::operator()(std::size_t row, std::size_t column) noexcept
{
    static_assert(std::is_same_v<decltype(m), float[3][3]>);
    POMDOG_ASSERT_MESSAGE(row < 3, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 3, "column: out of range");
    return m[row][column];
}

const float& Matrix3x3::operator()(std::size_t row, std::size_t column) const noexcept
{
    static_assert(std::is_same_v<decltype(m), float[3][3]>);
    POMDOG_ASSERT_MESSAGE(row < 3, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 3, "column: out of range");
    return m[row][column];
}

void Matrix3x3::SetScale(const Vector3& scale) noexcept
{
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;
}

Vector3 Matrix3x3::GetScale() const noexcept
{
    return Vector3{m[0][0], m[1][1], m[2][2]};
}

Matrix3x3
Matrix3x3::CreateTranslation(const Vector2& position) noexcept
{
    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        position.x, position.y, 1.0f};
}

Matrix3x3
Matrix3x3::CreateScale(float scale) noexcept
{
    return Matrix3x3{
        scale, 0.0f, 0.0f,
        0.0f, scale, 0.0f,
        0.0f, 0.0f, scale};
}

Matrix3x3
Matrix3x3::CreateScale(const Vector3& scale) noexcept
{
    return Matrix3x3{
        scale.x, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f,
        0.0f, 0.0f, scale.z};
}

Matrix3x3
Matrix3x3::CreateRotationX(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle,
        0.0f, -sinAngle, cosAngle};
}

Matrix3x3
Matrix3x3::CreateRotationY(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix3x3{
        cosAngle, 0.0f, -sinAngle,
        0.0f, 1.0f, 0.0f,
        sinAngle, 0.0f, cosAngle};
}

Matrix3x3
Matrix3x3::CreateRotationZ(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix3x3{
        cosAngle, sinAngle, 0.0f,
        -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 1.0f};
}

Matrix3x3
Matrix3x3::CreateFromQuaternion(const Quaternion& quaternion)
{
    const auto xx = quaternion.x * quaternion.x;
    const auto yy = quaternion.y * quaternion.y;
    const auto zz = quaternion.z * quaternion.z;
    const auto xy = quaternion.x * quaternion.y;
    const auto zw = quaternion.z * quaternion.w;
    const auto zx = quaternion.z * quaternion.x;
    const auto yw = quaternion.y * quaternion.w;
    const auto yz = quaternion.y * quaternion.z;
    const auto xw = quaternion.x * quaternion.w;

    return Matrix3x3{
        1.0f - (2.0f * (yy + zz)),
        2.0f * (xy + zw),
        2.0f * (zx - yw),
        2.0f * (xy - zw),
        1.0f - (2.0f * (zz + xx)),
        2.0f * (yz + xw),
        2.0f * (zx + yw),
        2.0f * (yz - xw),
        1.0f - (2.0f * (yy + xx)),
    };
}

Matrix3x3
Matrix3x3::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    const auto t = 1.0f - cosAngle;
    const auto xx = axis.x * axis.x;
    const auto yy = axis.y * axis.y;
    const auto zz = axis.z * axis.z;
    const auto xy = axis.x * axis.y;
    const auto xz = axis.x * axis.z;
    const auto yz = axis.y * axis.z;

    // axis.Normalize();

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

const float* Matrix3x3::Data() const noexcept
{
    return m[0];
}

float* Matrix3x3::Data() noexcept
{
    return m[0];
}

[[nodiscard]] Matrix3x3
operator*(float factor, const Matrix3x3& matrix) noexcept
{
    return math::Multiply(matrix, factor);
}

Matrix3x3 Matrix3x3::Identity() noexcept
{
    return Matrix3x3{
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f};
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
Determinant(const Matrix3x3& matrix) noexcept
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
Minor2x2(const Matrix3x3& matrix, std::size_t row, std::size_t column)
{
    static_assert(std::is_same_v<decltype(matrix.m), float[3][3]>);
    constexpr std::size_t rowSize = 3;
    constexpr std::size_t columnSize = 3;
    POMDOG_ASSERT_MESSAGE(row < rowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < columnSize, "column: out of range");

    // NOTE:
    //   Matrix3x3 mat                     Matrix2x2 minor
    //     c0  c1  c2
    // r0 |11, 12, 13|                       |11, 13, x|
    // r1 |21, 22, 23| Minor2x2(mat, r2, c1) |21, 23. x|
    // r2 |31, 32, 33| --------------------> | x,  x, x|

#if defined(_MSC_VER) && !defined(NDEBUG)
    // NOTE: Avoid MSVC warning C4701: potentially uninitialized local variable 'minorMatrix' used
    auto minorMatrix = Matrix2x2::Identity();
#else
    Matrix2x2 minorMatrix;
#endif
    for (std::size_t i = 0, s = 0; i < rowSize; ++i) {
        if (row == i) {
            continue;
        }

        for (std::size_t j = 0, t = 0; j < columnSize; ++j) {
            if (column == j) {
                continue;
            }

            POMDOG_ASSERT(s < 2);
            POMDOG_ASSERT(t < 2);
            minorMatrix.m[s][t] = matrix.m[i][j];
            ++t;
        }
        ++s;
    }
    return minorMatrix;
}

[[nodiscard]] Matrix3x3
Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) noexcept
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
Multiply(const Matrix3x3& matrix1, float factor) noexcept
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
Adjoint(const Matrix3x3& matrix)
{
    return Matrix3x3{
        +math::Determinant(math::Minor2x2(matrix, 0, 0)),
        -math::Determinant(math::Minor2x2(matrix, 1, 0)),
        +math::Determinant(math::Minor2x2(matrix, 2, 0)),
        -math::Determinant(math::Minor2x2(matrix, 0, 1)),
        +math::Determinant(math::Minor2x2(matrix, 1, 1)),
        -math::Determinant(math::Minor2x2(matrix, 2, 1)),
        +math::Determinant(math::Minor2x2(matrix, 0, 2)),
        -math::Determinant(math::Minor2x2(matrix, 1, 2)),
        +math::Determinant(math::Minor2x2(matrix, 2, 2)),
    };
}

[[nodiscard]] Matrix3x3
Transpose(const Matrix3x3& matrix) noexcept
{
    return Matrix3x3{
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]};
}

[[nodiscard]] Matrix3x3
Invert(const Matrix3x3& matrix)
{
    const auto determinant = math::Determinant(matrix);

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    const auto inverseDet = 1.0f / determinant;
    return Adjoint(matrix) * inverseDet;
}

[[nodiscard]] Matrix3x3
Lerp(const Matrix3x3& source1, const Matrix3x3& source2, float amount) noexcept
{
    return Matrix3x3{
        math::Lerp(source1.m[0][0], source2.m[0][0], amount),
        math::Lerp(source1.m[0][1], source2.m[0][1], amount),
        math::Lerp(source1.m[0][2], source2.m[0][2], amount),
        math::Lerp(source1.m[1][0], source2.m[1][0], amount),
        math::Lerp(source1.m[1][1], source2.m[1][1], amount),
        math::Lerp(source1.m[1][2], source2.m[1][2], amount),
        math::Lerp(source1.m[2][0], source2.m[2][0], amount),
        math::Lerp(source1.m[2][1], source2.m[2][1], amount),
        math::Lerp(source1.m[2][2], source2.m[2][2], amount),
    };
}

} // namespace pomdog::math
