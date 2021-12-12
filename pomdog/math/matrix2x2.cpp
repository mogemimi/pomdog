// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix2x2.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Matrix2x2 const Matrix2x2::Identity = {
    1, 0, //
    0, 1, //
};

Matrix2x2::Matrix2x2() noexcept = default;

Matrix2x2::Matrix2x2(float m00, float m01, float m10, float m11) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[1][0] = m10;
    m[1][1] = m11;
}

Matrix2x2&
Matrix2x2::operator*=(const Matrix2x2& other) noexcept
{
    *this = Multiply(*this, other);
    return *this;
}

Matrix2x2&
Matrix2x2::operator+=(const Matrix2x2& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    return *this;
}

Matrix2x2&
Matrix2x2::operator-=(const Matrix2x2& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    return *this;
}

Matrix2x2&
Matrix2x2::operator*=(float scaleFactor) noexcept
{
    m[0][0] *= scaleFactor;
    m[0][1] *= scaleFactor;
    m[1][0] *= scaleFactor;
    m[1][1] *= scaleFactor;
    return *this;
}

Matrix2x2&
Matrix2x2::operator/=(float scaleFactor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    return *this;
}

Matrix2x2 Matrix2x2::operator+() const noexcept
{
    return *this;
}

Matrix2x2 Matrix2x2::operator-() const noexcept
{
    return {-m[0][0], -m[0][1], -m[1][0], -m[1][1]};
}

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1]};
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1]};
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& other) const noexcept
{
    return Multiply(*this, other);
}

Matrix2x2 Matrix2x2::operator*(float scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

Matrix2x2 Matrix2x2::operator/(float scaleFactor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider};
}

bool Matrix2x2::operator==(const Matrix2x2& other) const noexcept
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1];
}

bool Matrix2x2::operator!=(const Matrix2x2& other) const noexcept
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1];
}

float& Matrix2x2::operator()(std::size_t row, std::size_t column) noexcept
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

const float& Matrix2x2::operator()(std::size_t row, std::size_t column) const noexcept
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

Matrix2x2 Matrix2x2::Multiply(
    const Matrix2x2& matrix1,
    const Matrix2x2& matrix2) noexcept
{
    return {
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1]};
}

Matrix2x2 Matrix2x2::Multiply(
    const Matrix2x2& matrix1, float scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor};
}

float Matrix2x2::Determinant() const noexcept
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

const float* Matrix2x2::Data() const noexcept
{
    return m[0];
}

float* Matrix2x2::Data() noexcept
{
    return m[0];
}

[[nodiscard]] Matrix2x2
operator*(float scaleFactor, const Matrix2x2& matrix) noexcept
{
    return Matrix2x2::Multiply(matrix, scaleFactor);
}

} // namespace pomdog
