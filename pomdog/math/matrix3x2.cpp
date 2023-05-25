// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x2.h"
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

Matrix3x2::Matrix3x2() noexcept = default;

Matrix3x2::Matrix3x2(
    float m00, float m01,
    float m10, float m11,
    float m20, float m21) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[1][0] = m10;
    m[1][1] = m11;
    m[2][0] = m20;
    m[2][1] = m21;
}

Matrix3x2& Matrix3x2::operator*=(const Matrix3x2& other) noexcept
{
    *this = math::multiply(*this, other);
    return *this;
}

Matrix3x2& Matrix3x2::operator+=(const Matrix3x2& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    return *this;
}

Matrix3x2& Matrix3x2::operator-=(const Matrix3x2& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    return *this;
}

Matrix3x2& Matrix3x2::operator*=(float factor) noexcept
{
    m[0][0] *= factor;
    m[0][1] *= factor;
    m[1][0] *= factor;
    m[1][1] *= factor;
    m[2][0] *= factor;
    m[2][1] *= factor;
    return *this;
}

Matrix3x2& Matrix3x2::operator/=(float factor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    m[2][0] *= inverseDivider;
    m[2][1] *= inverseDivider;
    return *this;
}

Matrix3x2 Matrix3x2::operator+() const noexcept
{
    return *this;
}

Matrix3x2 Matrix3x2::operator-() const noexcept
{
    return math::multiply(*this, -1.0f);
}

Matrix3x2 Matrix3x2::operator+(const Matrix3x2& other) const noexcept
{
    return Matrix3x2{
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1],
    };
}

Matrix3x2 Matrix3x2::operator-(const Matrix3x2& other) const noexcept
{
    return Matrix3x2{
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1],
    };
}

Matrix3x2 Matrix3x2::operator*(const Matrix3x2& other) const noexcept
{
    return math::multiply(*this, other);
}

Matrix3x2 Matrix3x2::operator*(float factor) const noexcept
{
    return math::multiply(*this, factor);
}

Matrix3x2 Matrix3x2::operator/(float factor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(factor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(factor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / factor;
    return Matrix3x2{
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider,
    };
}

bool Matrix3x2::operator==(const Matrix3x2& other) const noexcept
{
    return m[0][0] == other.m[0][0] &&
           m[0][1] == other.m[0][1] &&
           m[1][0] == other.m[1][0] &&
           m[1][1] == other.m[1][1] &&
           m[2][0] == other.m[2][0] &&
           m[2][1] == other.m[2][1];
}

bool Matrix3x2::operator!=(const Matrix3x2& other) const noexcept
{
    return m[0][0] != other.m[0][0] ||
           m[0][1] != other.m[0][1] ||
           m[1][0] != other.m[1][0] ||
           m[1][1] != other.m[1][1] ||
           m[2][0] != other.m[2][0] ||
           m[2][1] != other.m[2][1];
}

float& Matrix3x2::operator()(std::size_t row, std::size_t column) noexcept
{
    static_assert(std::is_same_v<decltype(m), float[3][2]>);
    POMDOG_ASSERT_MESSAGE(row < 3, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 2, "column: out of range");
    return m[row][column];
}

const float& Matrix3x2::operator()(std::size_t row, std::size_t column) const noexcept
{
    static_assert(std::is_same_v<decltype(m), float[3][2]>);
    POMDOG_ASSERT_MESSAGE(row < 3, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < 2, "column: out of range");
    return m[row][column];
}

Matrix3x2
Matrix3x2::createTranslation(const Vector2& position) noexcept
{
    return Matrix3x2{
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        position.x,
        position.y,
    };
}

Matrix3x2
Matrix3x2::createScale(float scale) noexcept
{
    return Matrix3x2{
        scale,
        0.0f,
        0.0f,
        scale,
        0.0f,
        0.0f,
    };
}

Matrix3x2
Matrix3x2::createScale(const Vector2& scale) noexcept
{
    return Matrix3x2{
        scale.x,
        0.0f,
        0.0f,
        scale.y,
        0.0f,
        0.0f,
    };
}

Matrix3x2
Matrix3x2::createRotation(const Radian<float>& angle)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    return Matrix3x2{
        cosAngle,
        sinAngle,
        -sinAngle,
        cosAngle,
        0.0f,
        0.0f,
    };
}

Matrix3x2
Matrix3x2::createSkew(const Vector2& skew)
{
    return Matrix3x2{
        1.0f,
        std::tan(skew.y),
        std::tan(skew.x),
        1.0f,
        0.0f,
        0.0f,
    };
}

const float* Matrix3x2::data() const noexcept
{
    static_assert(std::is_floating_point<float>::value, "T is floating point number");
    return m[0];
}

float* Matrix3x2::data() noexcept
{
    static_assert(std::is_floating_point<float>::value, "T is floating point number");
    return m[0];
}

[[nodiscard]] Matrix3x2 Matrix3x2::createIdentity() noexcept
{
    return Matrix3x2{
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f};
}

[[nodiscard]] Matrix3x2
operator*(float factor, const Matrix3x2& matrix) noexcept
{
    return math::multiply(matrix, factor);
}

} // namespace pomdog

namespace pomdog::math {

[[nodiscard]] float
determinant(const Matrix3x2& matrix) noexcept
{
    return (matrix.m[0][0] * matrix.m[1][1]) - (matrix.m[0][1] * matrix.m[1][0]);
}

[[nodiscard]] Matrix3x2
multiply(const Matrix3x2& matrix1, const Matrix3x2& matrix2) noexcept
{
    return Matrix3x2{
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1],
        matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix2.m[2][0],
        matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix2.m[2][1],
    };
}

[[nodiscard]] Matrix3x2
multiply(const Matrix3x2& matrix1, float factor) noexcept
{
    return Matrix3x2{
        matrix1.m[0][0] * factor,
        matrix1.m[0][1] * factor,
        matrix1.m[1][0] * factor,
        matrix1.m[1][1] * factor,
        matrix1.m[2][0] * factor,
        matrix1.m[2][1] * factor,
    };
}

[[nodiscard]] Matrix3x2
invert(const Matrix3x2& matrix)
{
    const auto det = math::determinant(matrix);

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(det) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(det) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(det) != FP_INFINITE);

    const auto inverseDet = 1.0f / det;
    const auto offsetX = matrix(2, 0);
    const auto offsetY = matrix(2, 1);

    return Matrix3x2{
        matrix(1, 1) * inverseDet,
        -matrix(0, 1) * inverseDet,
        -matrix(1, 0) * inverseDet,
        matrix(0, 0) * inverseDet,
        (matrix(1, 0) * offsetY - offsetX * matrix(1, 1)) * inverseDet,
        (offsetX * matrix(0, 1) - matrix(0, 0) * offsetY) * inverseDet,
    };
}

[[nodiscard]] Matrix3x2
lerp(const Matrix3x2& source1, const Matrix3x2& source2, float amount) noexcept
{
    return Matrix3x2{
        math::lerp(source1.m[0][0], source2.m[0][0], amount),
        math::lerp(source1.m[0][1], source2.m[0][1], amount),
        math::lerp(source1.m[1][0], source2.m[1][0], amount),
        math::lerp(source1.m[1][1], source2.m[1][1], amount),
        math::lerp(source1.m[2][0], source2.m[2][0], amount),
        math::lerp(source1.m[2][1], source2.m[2][1], amount),
    };
}

} // namespace pomdog::math
