// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/matrix3x3.hpp"
#include "pomdog/math/quaternion.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector3.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Matrix4x4 const Matrix4x4::Identity = {
    1, 0, 0, 0, //
    0, 1, 0, 0, //
    0, 0, 1, 0, //
    0, 0, 0, 1, //
};

Matrix4x4::Matrix4x4() noexcept = default;

Matrix4x4::Matrix4x4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

const float& Matrix4x4::operator()(std::size_t row, std::size_t column) const noexcept
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

float& Matrix4x4::operator()(std::size_t row, std::size_t column) noexcept
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

//
//const std::array<T, 4>& Matrix4x4::operator[](std::size_t row) const
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "Matrix4x4, subscript out of range");
//    return m[row];
//}

//
//std::array<T, 4> & Matrix4x4::operator[](std::size_t row)
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "Matrix4x4, subscript out of range");
//    return m[row];
//}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other) noexcept
{
    *this = Multiply(*this, other);
    return *this;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[0][2] += other.m[0][2];
    m[0][3] += other.m[0][3];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[1][2] += other.m[1][2];
    m[1][3] += other.m[1][3];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    m[2][2] += other.m[2][2];
    m[2][3] += other.m[2][3];
    m[3][0] += other.m[3][0];
    m[3][1] += other.m[3][1];
    m[3][2] += other.m[3][2];
    m[3][3] += other.m[3][3];
    return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[0][2] -= other.m[0][2];
    m[0][3] -= other.m[0][3];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[1][2] -= other.m[1][2];
    m[1][3] -= other.m[1][3];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    m[2][2] -= other.m[2][2];
    m[2][3] -= other.m[2][3];
    m[3][0] -= other.m[3][0];
    m[3][1] -= other.m[3][1];
    m[3][2] -= other.m[3][2];
    m[3][3] -= other.m[3][3];
    return *this;
}

Matrix4x4& Matrix4x4::operator*=(float scaleFactor) noexcept
{
    *this = Multiply(*this, scaleFactor);
    return *this;
}

Matrix4x4& Matrix4x4::operator/=(float scaleFactor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[0][2] *= inverseDivider;
    m[0][3] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    m[1][2] *= inverseDivider;
    m[1][3] *= inverseDivider;
    m[2][0] *= inverseDivider;
    m[2][1] *= inverseDivider;
    m[2][2] *= inverseDivider;
    m[2][3] *= inverseDivider;
    m[3][0] *= inverseDivider;
    m[3][1] *= inverseDivider;
    m[3][2] *= inverseDivider;
    m[3][3] *= inverseDivider;
    return *this;
}

Matrix4x4 Matrix4x4::operator+() const noexcept
{
    return *this;
}

Matrix4x4 Matrix4x4::operator-() const noexcept
{
    return Multiply(*this, -1);
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[0][2] + other.m[0][2],
        m[0][3] + other.m[0][3],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[1][2] + other.m[1][2],
        m[1][3] + other.m[1][3],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1],
        m[2][2] + other.m[2][2],
        m[2][3] + other.m[2][3],
        m[3][0] + other.m[3][0],
        m[3][1] + other.m[3][1],
        m[3][2] + other.m[3][2],
        m[3][3] + other.m[3][3]};
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[0][2] - other.m[0][2],
        m[0][3] - other.m[0][3],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[1][2] - other.m[1][2],
        m[1][3] - other.m[1][3],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1],
        m[2][2] - other.m[2][2],
        m[2][3] - other.m[2][3],
        m[3][0] - other.m[3][0],
        m[3][1] - other.m[3][1],
        m[3][2] - other.m[3][2],
        m[3][3] - other.m[3][3]};
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const noexcept
{
    return Multiply(*this, other);
}

Matrix4x4 Matrix4x4::operator*(float scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

Matrix4x4 Matrix4x4::operator/(float scaleFactor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[0][2] * inverseDivider,
        m[0][3] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[1][2] * inverseDivider,
        m[1][3] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider,
        m[2][2] * inverseDivider,
        m[2][3] * inverseDivider,
        m[3][0] * inverseDivider,
        m[3][1] * inverseDivider,
        m[3][2] * inverseDivider,
        m[3][3] * inverseDivider};
}

bool Matrix4x4::operator==(const Matrix4x4& other) const noexcept
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[0][2] == other.m[0][2] &&
           m[0][3] == other.m[0][3] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1] &&
           m[1][2] == other.m[1][2] &&
           m[1][3] == other.m[1][3] &&
           m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1] &&
           m[2][2] == other.m[2][2] &&
           m[2][3] == other.m[2][3] &&
           m[3][0] == other.m[3][0] &&
           m[3][1] == other.m[3][1] &&
           m[3][2] == other.m[3][2] &&
           m[3][3] == other.m[3][3];
}

bool Matrix4x4::operator!=(const Matrix4x4& other) const noexcept
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[0][2] != other.m[0][2] ||
           m[0][3] != other.m[0][3] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1] ||
           m[1][2] != other.m[1][2] ||
           m[1][3] != other.m[1][3] ||
           m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1] ||
           m[2][2] != other.m[2][2] ||
           m[2][3] != other.m[2][3] ||
           m[3][0] != other.m[3][0] ||
           m[3][1] != other.m[3][1] ||
           m[3][2] != other.m[3][2] ||
           m[3][3] != other.m[3][3];
}

Matrix4x4 Matrix4x4::Multiply(
    const Matrix4x4& matrix1,
    const Matrix4x4& matrix2) noexcept
{
    const auto& a = matrix1;
    const auto& b = matrix2;
    return {
        a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0] + a.m[0][3] * b.m[3][0],
        a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1] + a.m[0][3] * b.m[3][1],
        a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2] + a.m[0][3] * b.m[3][2],
        a.m[0][0] * b.m[0][3] + a.m[0][1] * b.m[1][3] + a.m[0][2] * b.m[2][3] + a.m[0][3] * b.m[3][3],

        a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0] + a.m[1][3] * b.m[3][0],
        a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1] + a.m[1][3] * b.m[3][1],
        a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2] + a.m[1][3] * b.m[3][2],
        a.m[1][0] * b.m[0][3] + a.m[1][1] * b.m[1][3] + a.m[1][2] * b.m[2][3] + a.m[1][3] * b.m[3][3],

        a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0] + a.m[2][3] * b.m[3][0],
        a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1] + a.m[2][3] * b.m[3][1],
        a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2] + a.m[2][3] * b.m[3][2],
        a.m[2][0] * b.m[0][3] + a.m[2][1] * b.m[1][3] + a.m[2][2] * b.m[2][3] + a.m[2][3] * b.m[3][3],

        a.m[3][0] * b.m[0][0] + a.m[3][1] * b.m[1][0] + a.m[3][2] * b.m[2][0] + a.m[3][3] * b.m[3][0],
        a.m[3][0] * b.m[0][1] + a.m[3][1] * b.m[1][1] + a.m[3][2] * b.m[2][1] + a.m[3][3] * b.m[3][1],
        a.m[3][0] * b.m[0][2] + a.m[3][1] * b.m[1][2] + a.m[3][2] * b.m[2][2] + a.m[3][3] * b.m[3][2],
        a.m[3][0] * b.m[0][3] + a.m[3][1] * b.m[1][3] + a.m[3][2] * b.m[2][3] + a.m[3][3] * b.m[3][3]};
}

Matrix4x4 Matrix4x4::Multiply(
    const Matrix4x4& matrix1, float scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[0][2] * scaleFactor,
        matrix1.m[0][3] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor,
        matrix1.m[1][2] * scaleFactor,
        matrix1.m[1][3] * scaleFactor,
        matrix1.m[2][0] * scaleFactor,
        matrix1.m[2][1] * scaleFactor,
        matrix1.m[2][2] * scaleFactor,
        matrix1.m[2][3] * scaleFactor,
        matrix1.m[3][0] * scaleFactor,
        matrix1.m[3][1] * scaleFactor,
        matrix1.m[3][2] * scaleFactor,
        matrix1.m[3][3] * scaleFactor};
}

void Matrix4x4::SetTranslation(const Vector3& position) noexcept
{
    m[3][0] = position.X;
    m[3][1] = position.Y;
    m[3][2] = position.Z;
}

Vector3 Matrix4x4::GetTranslation() const noexcept
{
    return {m[3][0], m[3][1], m[3][2]};
}

void Matrix4x4::SetScale(const Vector3& scale) noexcept
{
    m[0][0] = scale.X;
    m[1][1] = scale.Y;
    m[2][2] = scale.Z;
}

Vector3 Matrix4x4::GetScale() const noexcept
{
    return {m[0][0], m[1][1], m[2][2]};
}

float Matrix4x4::Determinant() const noexcept
{
    // TODO: Replace with simple process using cofactors to compute a determinant
    // please see: Matrix3x3::Determinant implementation
    return m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2] +
           m[0][1] * m[1][0] * m[2][3] * m[3][2] + m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][1] * m[1][3] * m[2][2] * m[3][0] +
           m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][2] * m[1][3] * m[2][0] * m[3][1] +
           m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] + m[0][3] * m[1][2] * m[2][1] * m[3][0] -
           m[0][0] * m[1][1] * m[2][3] * m[3][2] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][3] * m[2][2] * m[3][1] -
           m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][1] * m[1][3] * m[2][0] * m[3][2] -
           m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][2] * m[1][3] * m[2][1] * m[3][0] -
           m[0][3] * m[1][0] * m[2][1] * m[3][2] - m[0][3] * m[1][1] * m[2][2] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1];
}

Matrix3x3 Matrix4x4::Minor3x3(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");

    //      4x4 matrix                              3x3 minor
    //     c0  c1  c2  c3
    // r0 |11, 12, 13, 14|                       |11, 12, 13, x|
    // r1 |21, 22, 23, 24| Minor3x3(mat, r2, c3) |21, 22. 23, x|
    // r2 |31, 32, 33, 34| --------------------> |41, 42, 43, x|
    // r3 |41, 42, 43, 44|                       | x,  x,  x, x|

#if defined(_MSC_VER) && !defined(NDEBUG)
    // NOTE: Avoid MSVC warning C4701: potentially uninitialized local variable 'minorMatrix' used
    auto minorMatrix = Matrix3x3::Identity;
#else
    Matrix3x3 minorMatrix;
#endif
    for (std::size_t i = 0, s = 0; i < 4; ++i) {
        if (row == i) {
            continue;
        }

        for (std::size_t j = 0, t = 0; j < 4; ++j) {
            if (column == j) {
                continue;
            }

            minorMatrix(s, t) = this->m[i][j];
            ++t;
        }
        ++s;
    }
    return minorMatrix;
}

Matrix4x4
Matrix4x4::Adjoint(const Matrix4x4& matrix)
{
    return {
        +matrix.Minor3x3(0, 0).Determinant(),
        -matrix.Minor3x3(1, 0).Determinant(),
        +matrix.Minor3x3(2, 0).Determinant(),
        -matrix.Minor3x3(3, 0).Determinant(),
        -matrix.Minor3x3(0, 1).Determinant(),
        +matrix.Minor3x3(1, 1).Determinant(),
        -matrix.Minor3x3(2, 1).Determinant(),
        +matrix.Minor3x3(3, 1).Determinant(),
        +matrix.Minor3x3(0, 2).Determinant(),
        -matrix.Minor3x3(1, 2).Determinant(),
        +matrix.Minor3x3(2, 2).Determinant(),
        -matrix.Minor3x3(3, 2).Determinant(),
        -matrix.Minor3x3(0, 3).Determinant(),
        +matrix.Minor3x3(1, 3).Determinant(),
        -matrix.Minor3x3(2, 3).Determinant(),
        +matrix.Minor3x3(3, 3).Determinant()};
}

void Matrix4x4::Transpose(const Matrix4x4& matrix, Matrix4x4& result) noexcept
{
    result = {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]};
}

Matrix4x4
Matrix4x4::Transpose(const Matrix4x4& matrix) noexcept
{
    return {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]};
}

void Matrix4x4::Invert(const Matrix4x4& matrix, Matrix4x4& result)
{
    const auto determinant = matrix.Determinant();

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    const auto inverseDet = 1.0f / determinant;
    result = Adjoint(matrix) * inverseDet;
}

Matrix4x4
Matrix4x4::Invert(const Matrix4x4& matrix)
{
    Matrix4x4 result;
    Invert(matrix, result);
    return result;
}

void Matrix4x4::Lerp(const Matrix4x4& source1, const Matrix4x4& source2,
    float amount, Matrix4x4& result) noexcept
{
    result.m[0][0] = math::Lerp(source1.m[0][0], source2.m[0][0], amount);
    result.m[0][1] = math::Lerp(source1.m[0][1], source2.m[0][1], amount);
    result.m[0][2] = math::Lerp(source1.m[0][2], source2.m[0][2], amount);
    result.m[0][3] = math::Lerp(source1.m[0][3], source2.m[0][3], amount);
    result.m[1][0] = math::Lerp(source1.m[1][0], source2.m[1][0], amount);
    result.m[1][1] = math::Lerp(source1.m[1][1], source2.m[1][1], amount);
    result.m[1][2] = math::Lerp(source1.m[1][2], source2.m[1][2], amount);
    result.m[1][3] = math::Lerp(source1.m[1][3], source2.m[1][3], amount);
    result.m[2][0] = math::Lerp(source1.m[2][0], source2.m[2][0], amount);
    result.m[2][1] = math::Lerp(source1.m[2][1], source2.m[2][1], amount);
    result.m[2][2] = math::Lerp(source1.m[2][2], source2.m[2][2], amount);
    result.m[2][3] = math::Lerp(source1.m[2][3], source2.m[2][3], amount);
    result.m[3][0] = math::Lerp(source1.m[3][0], source2.m[3][0], amount);
    result.m[3][1] = math::Lerp(source1.m[3][1], source2.m[3][1], amount);
    result.m[3][2] = math::Lerp(source1.m[3][2], source2.m[3][2], amount);
    result.m[3][3] = math::Lerp(source1.m[3][3], source2.m[3][3], amount);
}

Matrix4x4
Matrix4x4::Lerp(const Matrix4x4& source1, const Matrix4x4& source2,
    float amount) noexcept
{
    Matrix4x4 result;
    Lerp(source1, source2, amount, result);
    return result;
}

void Matrix4x4::CreateTranslation(const Vector3& position, Matrix4x4& result) noexcept
{
    result = Identity;
    result.m[3][0] = position.X;
    result.m[3][1] = position.Y;
    result.m[3][2] = position.Z;
}

Matrix4x4
Matrix4x4::CreateTranslation(const Vector3& position) noexcept
{
    Matrix4x4 result;
    CreateTranslation(position, result);
    return result;
}

void Matrix4x4::CreateScale(float scale, Matrix4x4& result) noexcept
{
    result = Identity;
    result.m[0][0] = scale;
    result.m[1][1] = scale;
    result.m[2][2] = scale;
}

Matrix4x4
Matrix4x4::CreateScale(float scale) noexcept
{
    Matrix4x4 result;
    CreateScale(scale, result);
    return result;
}

void Matrix4x4::CreateScale(const Vector3& scale, Matrix4x4& result) noexcept
{
    result = Identity;
    result.m[0][0] = scale.X;
    result.m[1][1] = scale.Y;
    result.m[2][2] = scale.Z;
}

Matrix4x4
Matrix4x4::CreateScale(const Vector3& scale) noexcept
{
    Matrix4x4 result;
    CreateScale(scale, result);
    return result;
}

void Matrix4x4::CreateRotationX(const Radian<float>& angle, Matrix4x4& result)
{
    result = Identity;
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[1][1] = cosAngle;
    result.m[1][2] = sinAngle;
    result.m[2][1] = -sinAngle;
    result.m[2][2] = cosAngle;
}

Matrix4x4
Matrix4x4::CreateRotationX(const Radian<float>& angle)
{
    Matrix4x4 result;
    CreateRotationX(angle, result);
    return result;
}

void Matrix4x4::CreateRotationY(const Radian<float>& angle, Matrix4x4& result)
{
    result = Identity;
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][2] = -sinAngle;
    result.m[2][0] = sinAngle;
    result.m[2][2] = cosAngle;
}

Matrix4x4
Matrix4x4::CreateRotationY(const Radian<float>& angle)
{
    Matrix4x4 result;
    CreateRotationY(angle, result);
    return result;
}

void Matrix4x4::CreateRotationZ(const Radian<float>& angle, Matrix4x4& result)
{
    result = Identity;
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = sinAngle;
    result.m[1][0] = -sinAngle;
    result.m[1][1] = cosAngle;
}

Matrix4x4
Matrix4x4::CreateRotationZ(const Radian<float>& angle)
{
    Matrix4x4 result;
    CreateRotationZ(angle, result);
    return result;
}

void Matrix4x4::CreateFromQuaternion(const Quaternion& quaternion, Matrix4x4& result)
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
    result.m[0][3] = 0;

    result.m[1][0] = 2 * (xy - zw);
    result.m[1][1] = 1 - (2 * (zz + xx));
    result.m[1][2] = 2 * (yz + xw);
    result.m[1][3] = 0;

    result.m[2][0] = 2 * (zx + yw);
    result.m[2][1] = 2 * (yz - xw);
    result.m[2][2] = 1 - (2 * (yy + xx));
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateFromQuaternion(const Quaternion& quaternion)
{
    Matrix4x4 result;
    CreateFromQuaternion(quaternion, result);
    return result;
}

void Matrix4x4::CreateLookAtLH(
    const Vector3& eye,
    const Vector3& at,
    const Vector3& up,
    Matrix4x4& result)
{
    // zaxis = normal(At - Eye)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    const auto zaxis = Vector3::Normalize(at - eye); // RH: eye - at
    const auto xaxis = Vector3::Normalize(Vector3::Cross(up, zaxis));
    const auto yaxis = Vector3::Cross(zaxis, xaxis);

    result.m[0][0] = xaxis.X;
    result.m[0][1] = yaxis.X;
    result.m[0][2] = zaxis.X;
    result.m[0][3] = 0;

    result.m[1][0] = xaxis.Y;
    result.m[1][1] = yaxis.Y;
    result.m[1][2] = zaxis.Y;
    result.m[1][3] = 0;

    result.m[2][0] = xaxis.Z;
    result.m[2][1] = yaxis.Z;
    result.m[2][2] = zaxis.Z;
    result.m[2][3] = 0;

    result.m[3][0] = -Vector3::Dot(xaxis, eye);
    result.m[3][1] = -Vector3::Dot(yaxis, eye);
    result.m[3][2] = -Vector3::Dot(zaxis, eye);
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateLookAtLH(const Vector3& eye, const Vector3& at,
    const Vector3& up)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreateLookAtLH(eye, at, up, result);
    return result;
}

void Matrix4x4::CreateLookAtRH(const Vector3& eye, const Vector3& at,
    const Vector3& up, Matrix4x4& result)
{
    // zaxis = normal(Eye - At)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    const auto zaxis = Vector3::Normalize(eye - at); // LH: at - eye
    const auto xaxis = Vector3::Normalize(Vector3::Cross(up, zaxis));
    const auto yaxis = Vector3::Cross(zaxis, xaxis);

    result.m[0][0] = xaxis.X;
    result.m[0][1] = yaxis.X;
    result.m[0][2] = zaxis.X;
    result.m[0][3] = 0;

    result.m[1][0] = xaxis.Y;
    result.m[1][1] = yaxis.Y;
    result.m[1][2] = zaxis.Y;
    result.m[1][3] = 0;

    result.m[2][0] = xaxis.Z;
    result.m[2][1] = yaxis.Z;
    result.m[2][2] = zaxis.Z;
    result.m[2][3] = 0;

    result.m[3][0] = -Vector3::Dot(xaxis, eye);
    result.m[3][1] = -Vector3::Dot(yaxis, eye);
    result.m[3][2] = -Vector3::Dot(zaxis, eye);
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateLookAtRH(const Vector3& eye, const Vector3& at,
    const Vector3& up)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    Matrix4x4::CreateLookAtRH(eye, at, up, result);
    return result;
}

void Matrix4x4::CreatePerspectiveLH(float width, float height, float zNear, float zFar, Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(width > 0, "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0, "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zf-zn)     1
    // 0       0       zn*zf/(zn-zf)  0

    float const a = 2 * zNear / width;
    float const b = 2 * zNear / height;
    float const c = zFar / (zFar - zNear); // RH: c = zFar / (zNear - zFar);
    float const d = zNear * zFar / (zNear - zFar);

    result.m[0][0] = a;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = b;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = c;
    result.m[2][3] = 1; // RH: -1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveLH(float width, float height, float zNear, float zFar)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveLH(width, height, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreatePerspectiveRH(float width, float height, float zNear, float zFar, Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(width > 0, "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0, "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zn-zf)    -1
    // 0       0       zn*zf/(zn-zf)  0

    float const a = 2.0f * zNear / width;
    float const b = 2.0f * zNear / height;
    float const c = zFar / (zNear - zFar); //LH: c = zFar / (zFar - zNear);
    float const d = zNear * zFar / (zNear - zFar);

    result.m[0][0] = a;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = b;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = c;
    result.m[2][3] = -1; // LH: +1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveRH(float width, float height, float zNear, float zFar)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveRH(width, height, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreatePerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar,
    Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE:
    // f = cotangent(fovY/2)
    // cotangent(fovY/2) = 1/tan(fovY/2)
    //
    // The generated matrix is:
    // (f/Aspect)  0   0               0
    // 0           f   0               0
    // 0           0   zf/(zf-zn)      1
    // 0           0   -zn*zf/(zf-zn)  0

    float const yScale = 1.0f / std::tan(fovy.value / 2.0f);
    float const xScale = yScale / aspect;
    float const p = zFar / (zFar - zNear);

    result.m[0][0] = xScale;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = yScale;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = p;
    result.m[2][3] = 1;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = -zNear * p;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveFieldOfViewLH(fovy, aspect, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreatePerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar,
    Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // NOTE:
    // f = cotangent(fovY/2)
    // cotangent(fovY/2) = 1/tan(fovY/2)
    //
    // The generated matrix is:
    // (f/Aspect)  0   0               0
    // 0           f   0               0
    // 0           0   zf/(zn-zf)     -1
    // 0           0   zn*zf/(zn-zf)   0

    float const yScale = 1 / std::tan(fovy.value / 2);
    float const xScale = yScale / aspect;
    float const p = zFar / (zNear - zFar);

    result.m[0][0] = xScale;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = yScale;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = p;
    result.m[2][3] = -1;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = zNear * p;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveFieldOfViewRH(fovy, aspect, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreatePerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar,
    Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
    // 0            0            zn*zf/(zn-zf)  0

    float const x = 2 * zNear / (right - left);
    float const y = 2 * zNear / (top - bottom);
    float const a = (left + right) / (left - right); // RH: (left + right) / (right - left)
    float const b = (top + bottom) / (bottom - top); // RH: (top + bottom) / (top - bottom)
    float const c = zFar / (zFar - zNear);           // RH: zFar / (zNear - zFar)
    float const d = zNear * zFar / (zNear - zFar);

    result.m[0][0] = x;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = y;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = a;
    result.m[2][1] = b;
    result.m[2][2] = c;
    result.m[2][3] = 1; // RH: -1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveOffCenterLH(left, right, bottom, top, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreatePerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar,
    Matrix4x4& result)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)    -1
    // 0            0            zn*zf/(zn-zf)  0

    float const x = 2 * zNear / (right - left);
    float const y = 2 * zNear / (top - bottom);
    float const a = (left + right) / (right - left); // LH: (left + right) / (left - right)
    float const b = (top + bottom) / (top - bottom); // LH: (top + bottom) / (bottom - top)
    float const c = zFar / (zNear - zFar);           // LH: zFar / (zFar - zNear)
    float const d = zNear * zFar / (zNear - zFar);

    result.m[0][0] = x;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = y;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = a;
    result.m[2][1] = b;
    result.m[2][2] = c;
    result.m[2][3] = -1; // LH: +1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

Matrix4x4
Matrix4x4::CreatePerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    CreatePerspectiveOffCenterRH(left, right, bottom, top, zNear, zFar, result);
    return result;
}

void Matrix4x4::CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane,
    Matrix4x4& result)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE:
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zf-zn)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    float const inverseRightLeft = 1 / (right - left);
    float const inverseTopBottom = 1 / (top - bottom);
    float const inverseFarNear = 1 / (zFarPlane - zNearPlane);

    result.m[0][0] = 2 * inverseRightLeft;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2 * inverseTopBottom;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = inverseFarNear;
    result.m[2][3] = 0;

    result.m[3][0] = (left + right) * (-inverseRightLeft);
    result.m[3][1] = (top + bottom) * (-inverseTopBottom);
    result.m[3][2] = zNearPlane * (-inverseFarNear);
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreateOrthographicOffCenterLH(left, right, bottom, top, zNearPlane, zFarPlane, result);
    return result;
}

void Matrix4x4::CreateOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane,
    Matrix4x4& result)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE:
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zn-zf)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    float const inverseRightLeft = 1 / (right - left);
    float const inverseTopBottom = 1 / (top - bottom);
    float const inverseFarNear = 1 / (zNearPlane - zFarPlane);

    result.m[0][0] = 2 * inverseRightLeft;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 2 * inverseTopBottom;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = inverseFarNear;
    result.m[2][3] = 0;

    result.m[3][0] = (left + right) * (-inverseRightLeft);
    result.m[3][1] = (top + bottom) * (-inverseTopBottom);
    result.m[3][2] = zNearPlane * inverseFarNear;
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    CreateOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane, result);
    return result;
}

void Matrix4x4::CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane, Matrix4x4& result)
{
    CreateOrthographicOffCenterLH(
        -width / 2, width / 2, -height / 2, height / 2, zNearPlane, zFarPlane, result);
}

Matrix4x4
Matrix4x4::CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane)
{
    // NOTE: Left-handed Cartesian Coordinates
    Matrix4x4 result;
    CreateOrthographicLH(width, height, zNearPlane, zFarPlane, result);
    return result;
}

void Matrix4x4::CreateOrthographicRH(float width, float height, float zNearPlane, float zFarPlane, Matrix4x4& result)
{
    CreateOrthographicOffCenterRH(
        -width / 2, width / 2, -height / 2, height / 2, zNearPlane, zFarPlane, result);
}

Matrix4x4
Matrix4x4::CreateOrthographicRH(float width, float height, float zNearPlane, float zFarPlane)
{
    // NOTE: Right-handed Cartesian Coordinates
    Matrix4x4 result;
    CreateOrthographicRH(width, height, zNearPlane, zFarPlane, result);
    return result;
}

void Matrix4x4::CreateFromAxisAngle(
    const Vector3& axis, const Radian<float>& angle, Matrix4x4& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);
    const auto scaleFactor = 1.0f - cosAngle;
    const auto xx = axis.X * axis.X;
    const auto yy = axis.Y * axis.Y;
    const auto zz = axis.Z * axis.Z;
    const auto xy = axis.X * axis.Y;
    const auto xz = axis.X * axis.Z;
    const auto yz = axis.Y * axis.Z;

    result.m[0][0] = scaleFactor * xx + cosAngle;
    result.m[0][1] = scaleFactor * xy + sinAngle * axis.Z;
    result.m[0][2] = scaleFactor * xz - sinAngle * axis.Y;
    result.m[0][3] = 0;
    result.m[1][0] = scaleFactor * xy - sinAngle * axis.Z;
    result.m[1][1] = scaleFactor * yy + cosAngle;
    result.m[1][2] = scaleFactor * yz + sinAngle * axis.X;
    result.m[1][3] = 0;
    result.m[2][0] = scaleFactor * xz + sinAngle * axis.Y;
    result.m[2][1] = scaleFactor * yz - sinAngle * axis.X;
    result.m[2][2] = scaleFactor * zz + cosAngle;
    result.m[2][3] = 0;
    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;
}

Matrix4x4
Matrix4x4::CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle)
{
    Matrix4x4 result;
    CreateFromAxisAngle(axis, angle, result);
    return result;
}

const float* Matrix4x4::Data() const noexcept
{
    return m[0];
}

float* Matrix4x4::Data() noexcept
{
    return m[0];
}

[[nodiscard]] Matrix4x4
operator*(float scaleFactor, const Matrix4x4& matrix) noexcept
{
    return Matrix4x4::Multiply(matrix, scaleFactor);
}

} // namespace pomdog
