// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x3.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix2x2.hpp"
#include "pomdog/math/quaternion.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Matrix3x3 const Matrix3x3::Identity = {
    1, 0, 0, //
    0, 1, 0, //
    0, 0, 1, //
};

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
    *this = Multiply(*this, other);
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

Matrix3x3& Matrix3x3::operator*=(float scaleFactor) noexcept
{
    m[0][0] *= scaleFactor;
    m[0][1] *= scaleFactor;
    m[0][2] *= scaleFactor;
    m[1][0] *= scaleFactor;
    m[1][1] *= scaleFactor;
    m[1][2] *= scaleFactor;
    m[2][0] *= scaleFactor;
    m[2][1] *= scaleFactor;
    m[2][2] *= scaleFactor;
    return *this;
}

Matrix3x3& Matrix3x3::operator/=(float scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0.0f);
    const auto inverseDivider = 1.0f / scaleFactor;
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
    return Multiply(*this, -1.0f);
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[0][2] + other.m[0][2],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[1][2] + other.m[1][2],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1],
        m[2][2] + other.m[2][2]};
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[0][2] - other.m[0][2],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[1][2] - other.m[1][2],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1],
        m[2][2] - other.m[2][2]};
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const noexcept
{
    return Multiply(*this, other);
}

Matrix3x3 Matrix3x3::operator*(float scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

Matrix3x3 Matrix3x3::operator/(float scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0.0f);
    const auto inverseDivider = 1.0f / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[0][2] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[1][2] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider,
        m[2][2] * inverseDivider};
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

float& Matrix3x3::operator()(std::size_t row, std::size_t column)
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

const float& Matrix3x3::operator()(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

//
//const std::array<T, 3>& Matrix3x3::operator[](std::size_t row) const
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "Matrix3x3, subscript out of range");
//    return m[row];
//}

//
//std::array<T, 3> & Matrix3x3::operator[](std::size_t row)
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "Matrix3x3, subscript out of range");
//    return m[row];
//}

void Matrix3x3::SetScale(const Vector3& scale) noexcept
{
    m[0][0] = scale.X;
    m[1][1] = scale.Y;
    m[2][2] = scale.Z;
}

Vector3 Matrix3x3::GetScale() const noexcept
{
    return {m[0][0], m[1][1], m[2][2]};
}

float Matrix3x3::Determinant() const noexcept
{
    // | m00 m01 m02 | determinant =
    // | m10 m11 m12 |   (m00 * m11 * m22) +(m10 * m21 * m02) +(m20 *01 * m12)
    // | m20 m21 m22 |  -(m02 * m11 * m20) -(m12 * m21 * m00) -(m22 *01 * m10)
    //
    //   cofactor00    cofactor01     cofactor02
    //    |m11 m12|     |m10 m12|      |m10 m11|
    // m00|m21 m22|  m01|m20 m22|   m02|m20 m21|

    const auto cofactor00 = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
    const auto cofactor10 = m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]);
    const auto cofactor20 = m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    return cofactor00 + cofactor10 + cofactor20;
}

Matrix3x3
Matrix3x3::Adjoint(const Matrix3x3& matrix)
{
    return {
        matrix.Minor2x2(0, 0).Determinant(),
        -matrix.Minor2x2(1, 0).Determinant(),
        matrix.Minor2x2(2, 0).Determinant(),
        -matrix.Minor2x2(0, 1).Determinant(),
        matrix.Minor2x2(1, 1).Determinant(),
        -matrix.Minor2x2(2, 1).Determinant(),
        matrix.Minor2x2(0, 2).Determinant(),
        -matrix.Minor2x2(1, 2).Determinant(),
        matrix.Minor2x2(2, 2).Determinant()};
}

Matrix3x3 Matrix3x3::Multiply(
    const Matrix3x3& matrix1,
    const Matrix3x3& matrix2) noexcept
{
    return {
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] + matrix1.m[0][2] * matrix2.m[2][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] + matrix1.m[0][2] * matrix2.m[2][1],
        matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] + matrix1.m[0][2] * matrix2.m[2][2],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] + matrix1.m[1][2] * matrix2.m[2][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] + matrix1.m[1][2] * matrix2.m[2][1],
        matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] + matrix1.m[1][2] * matrix2.m[2][2],
        matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix1.m[2][2] * matrix2.m[2][0],
        matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix1.m[2][2] * matrix2.m[2][1],
        matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] + matrix1.m[2][2] * matrix2.m[2][2]};
}

Matrix3x3 Matrix3x3::Multiply(
    const Matrix3x3& matrix1, float scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[0][2] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor,
        matrix1.m[1][2] * scaleFactor,
        matrix1.m[2][0] * scaleFactor,
        matrix1.m[2][1] * scaleFactor,
        matrix1.m[2][2] * scaleFactor};
}

Matrix2x2 Matrix3x3::Minor2x2(std::size_t row, std::size_t column) const
{
    //   Matrix3x3 mat                     Matrix2x2 minor
    //     c0  c1  c2
    // r0 |11, 12, 13|                       |11, 13, x|
    // r1 |21, 22, 23| Minor2x2(mat, r2, c1) |21, 23. x|
    // r2 |31, 32, 33| --------------------> | x,  x, x|

#if defined(_MSC_VER) && !defined(NDEBUG)
    // NOTE: Avoid MSVC warning C4701: potentially uninitialized local variable 'minorMatrix' used
    auto minorMatrix = Matrix2x2::Identity;
#else
    Matrix2x2 minorMatrix;
#endif
    for (std::size_t i = 0, s = 0; i < RowSize; ++i) {
        if (row == i) {
            continue;
        }

        for (std::size_t j = 0, t = 0; j < ColumnSize; ++j) {
            if (column == j) {
                continue;
            }

            POMDOG_ASSERT(s < 2);
            POMDOG_ASSERT(t < 2);
            minorMatrix(s, t) = m[i][j];
            ++t;
        }
        ++s;
    }
    return minorMatrix;
}

void Matrix3x3::Transpose(const Matrix3x3& matrix, Matrix3x3& result) noexcept
{
    result = {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]};
}

Matrix3x3
Matrix3x3::Transpose(const Matrix3x3& matrix) noexcept
{
    return {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2]};
}

void Matrix3x3::Invert(const Matrix3x3& matrix, Matrix3x3& result)
{
    result = Invert(matrix);
}

Matrix3x3
Matrix3x3::Invert(const Matrix3x3& matrix)
{
    const auto determinant = matrix.Determinant();
    POMDOG_ASSERT_MESSAGE(0.0f != determinant, "This is singular matrix");

    return Adjoint(matrix) / determinant;
}

void Matrix3x3::CreateTranslation(const Vector2& position, Matrix3x3& result) noexcept
{
    result = Identity;
    result(2, 0) = position.X;
    result(2, 1) = position.Y;
}

Matrix3x3
Matrix3x3::CreateTranslation(const Vector2& position) noexcept
{
    Matrix3x3 result;
    CreateTranslation(position, result);
    return result;
}

void Matrix3x3::CreateScale(float scale, Matrix3x3& result) noexcept
{
    result.m[0][0] = scale;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale;
    result.m[1][2] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = scale;
}

Matrix3x3
Matrix3x3::CreateScale(float scale) noexcept
{
    Matrix3x3 result;
    CreateScale(scale, result);
    return result;
}

void Matrix3x3::CreateScale(const Vector3& scale, Matrix3x3& result) noexcept
{
    result.m[0][0] = scale.X;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale.Y;
    result.m[1][2] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = scale.Z;
}

Matrix3x3
Matrix3x3::CreateScale(const Vector3& scale) noexcept
{
    Matrix3x3 result;
    CreateScale(scale, result);
    return result;
}

void Matrix3x3::CreateRotationX(const Radian<float>& angle, Matrix3x3& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = cosAngle;
    result.m[1][2] = sinAngle;
    result.m[2][0] = 0;
    result.m[2][1] = -sinAngle;
    result.m[2][2] = cosAngle;
}

Matrix3x3
Matrix3x3::CreateRotationX(const Radian<float>& angle)
{
    Matrix3x3 result;
    CreateRotationX(angle, result);
    return result;
}

void Matrix3x3::CreateRotationY(const Radian<float>& angle, Matrix3x3& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = 0;
    result.m[0][2] = -sinAngle;
    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[2][0] = sinAngle;
    result.m[2][1] = 0;
    result.m[2][2] = cosAngle;
}

Matrix3x3
Matrix3x3::CreateRotationY(const Radian<float>& angle)
{
    Matrix3x3 result;
    CreateRotationY(angle, result);
    return result;
}

void Matrix3x3::CreateRotationZ(const Radian<float>& angle, Matrix3x3& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = sinAngle;
    result.m[0][2] = 0;
    result.m[1][0] = -sinAngle;
    result.m[1][1] = cosAngle;
    result.m[1][2] = 0;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
}

Matrix3x3
Matrix3x3::CreateRotationZ(const Radian<float>& angle)
{
    Matrix3x3 result;
    CreateRotationZ(angle, result);
    return result;
}

void Matrix3x3::CreateFromQuaternion(const Quaternion& quaternion, Matrix3x3& result)
{
    const auto xx = quaternion.X * quaternion.X;
    const auto yy = quaternion.Y * quaternion.Y;
    const auto zz = quaternion.Z * quaternion.Z;
    const auto xy = quaternion.X * quaternion.Y;
    const auto zw = quaternion.Z * quaternion.W;
    const auto zx = quaternion.Z * quaternion.X;
    const auto yw = quaternion.Y * quaternion.W;
    const auto yz = quaternion.Y * quaternion.Z;
    const auto xw = quaternion.X * quaternion.W;

    result.m[0][0] = 1 - (2 * (yy + zz));
    result.m[0][1] = 2 * (xy + zw);
    result.m[0][2] = 2 * (zx - yw);
    result.m[1][0] = 2 * (xy - zw);
    result.m[1][1] = 1 - (2 * (zz + xx));
    result.m[1][2] = 2 * (yz + xw);
    result.m[2][0] = 2 * (zx + yw);
    result.m[2][1] = 2 * (yz - xw);
    result.m[2][2] = 1 - (2 * (yy + xx));
}

Matrix3x3
Matrix3x3::CreateFromQuaternion(const Quaternion& quaternion)
{
    Matrix3x3 result;
    CreateFromQuaternion(quaternion, result);
    return result;
}

void Matrix3x3::Lerp(const Matrix3x3& source1,
    const Matrix3x3& source2, float amount, Matrix3x3& result) noexcept
{
    result.m[0][0] = math::Lerp(source1.m[0][0], source2.m[0][0], amount);
    result.m[0][1] = math::Lerp(source1.m[0][1], source2.m[0][1], amount);
    result.m[0][2] = math::Lerp(source1.m[0][2], source2.m[0][2], amount);
    result.m[1][0] = math::Lerp(source1.m[1][0], source2.m[1][0], amount);
    result.m[1][1] = math::Lerp(source1.m[1][1], source2.m[1][1], amount);
    result.m[1][2] = math::Lerp(source1.m[1][2], source2.m[1][2], amount);
    result.m[2][0] = math::Lerp(source1.m[2][0], source2.m[2][0], amount);
    result.m[2][1] = math::Lerp(source1.m[2][1], source2.m[2][1], amount);
    result.m[2][2] = math::Lerp(source1.m[2][2], source2.m[2][2], amount);
}

Matrix3x3
Matrix3x3::Lerp(const Matrix3x3& source1,
    const Matrix3x3& source2, float amount) noexcept
{
    Matrix3x3 result;
    Lerp(source1, source2, amount, result);
    return result;
}

void Matrix3x3::CreateFromAxisAngle(const Vector3& axis,
    const Radian<float>& angle, Matrix3x3& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    const auto t = 1.0f - cosAngle;
    const auto xx = axis.X * axis.X;
    const auto yy = axis.Y * axis.Y;
    const auto zz = axis.Z * axis.Z;
    const auto xy = axis.X * axis.Y;
    const auto xz = axis.X * axis.Z;
    const auto yz = axis.Y * axis.Z;

    //axis.Normalize();

    result.m[0][0] = t * xx + cosAngle;
    result.m[0][1] = t * xy + sinAngle * axis.Z;
    result.m[0][2] = t * xz - sinAngle * axis.Y;
    result.m[1][0] = t * xy - sinAngle * axis.Z;
    result.m[1][1] = t * yy + cosAngle;
    result.m[1][2] = t * yz + sinAngle * axis.X;
    result.m[2][0] = t * xz + sinAngle * axis.Y;
    result.m[2][1] = t * yz - sinAngle * axis.X;
    result.m[2][2] = t * zz + cosAngle;
}

Matrix3x3
Matrix3x3::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    Matrix3x3 result;
    CreateFromAxisAngle(axis, angle, result);
    return result;
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
operator*(float scaleFactor, const Matrix3x3& matrix) noexcept
{
    return Matrix3x3::Multiply(matrix, scaleFactor);
}

} // namespace pomdog
