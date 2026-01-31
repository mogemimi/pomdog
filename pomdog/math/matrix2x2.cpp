// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix2x2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <limits>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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
    *this = math::multiply(*this, other);
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
Matrix2x2::operator*=(float factor) noexcept
{
    m[0][0] *= factor;
    m[0][1] *= factor;
    m[1][0] *= factor;
    m[1][1] *= factor;
    return *this;
}

Matrix2x2&
Matrix2x2::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
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
    return Matrix2x2{-m[0][0], -m[0][1], -m[1][0], -m[1][1]};
}

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& other) const noexcept
{
    return Matrix2x2{
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
    };
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& other) const noexcept
{
    return Matrix2x2{
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
    };
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& other) const noexcept
{
    return math::multiply(*this, other);
}

Matrix2x2 Matrix2x2::operator*(float factor) const noexcept
{
    return math::multiply(*this, factor);
}

Matrix2x2 Matrix2x2::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    return Matrix2x2{
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
    };
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

float Matrix2x2::operator()(i32 row, i32 column) const noexcept
{
    static_assert(std::is_same_v<decltype(m), float[2][2]>);
    POMDOG_ASSERT_MESSAGE(row >= 0, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column >= 0, "column: out of range");
    POMDOG_ASSERT_MESSAGE(row < 2, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 2, "column: out of range");
    return m[row][column];
}

float& Matrix2x2::operator()(i32 row, i32 column) noexcept
{
    static_assert(std::is_same_v<decltype(m), float[2][2]>);
    POMDOG_ASSERT_MESSAGE(row >= 0, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column >= 0, "column: out of range");
    POMDOG_ASSERT_MESSAGE(row < 2, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 2, "column: out of range");
    return m[row][column];
}

const float* Matrix2x2::data() const noexcept
{
    return m[0];
}

float* Matrix2x2::data() noexcept
{
    return m[0];
}

Matrix2x2 Matrix2x2::createIdentity() noexcept
{
    return Matrix2x2{
        1, 0,
        0, 1};
}

[[nodiscard]] Matrix2x2
operator*(float factor, const Matrix2x2& matrix) noexcept
{
    return math::multiply(matrix, factor);
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
determinant(const Matrix2x2& matrix) noexcept
{
    return matrix.m[0][0] * matrix.m[1][1] - matrix.m[0][1] * matrix.m[1][0];
}

[[nodiscard]] Matrix2x2
multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2) noexcept
{
    return Matrix2x2{
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1],
    };
}

[[nodiscard]] Matrix2x2
multiply(const Matrix2x2& matrix1, float factor) noexcept
{
    return Matrix2x2{
        matrix1.m[0][0] * factor,
        matrix1.m[0][1] * factor,
        matrix1.m[1][0] * factor,
        matrix1.m[1][1] * factor,
    };
}

} // namespace pomdog::math
