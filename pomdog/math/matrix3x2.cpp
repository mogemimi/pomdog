// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/matrix3x2.hpp"
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

Matrix3x2 const Matrix3x2::Identity = {
    1, 0, //
    0, 1, //
    0, 0, //
};

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
    *this = Multiply(*this, other);
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

Matrix3x2& Matrix3x2::operator*=(float scaleFactor) noexcept
{
    m[0][0] *= scaleFactor;
    m[0][1] *= scaleFactor;
    m[1][0] *= scaleFactor;
    m[1][1] *= scaleFactor;
    m[2][0] *= scaleFactor;
    m[2][1] *= scaleFactor;
    return *this;
}

Matrix3x2& Matrix3x2::operator/=(float scaleFactor) noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
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
    return Multiply(*this, -1.0f);
}

Matrix3x2 Matrix3x2::operator+(const Matrix3x2& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1]};
}

Matrix3x2 Matrix3x2::operator-(const Matrix3x2& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1]};
}

Matrix3x2 Matrix3x2::operator*(const Matrix3x2& other) const noexcept
{
    return Multiply(*this, other);
}

Matrix3x2 Matrix3x2::operator*(float scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

Matrix3x2 Matrix3x2::operator/(float scaleFactor) const noexcept
{
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(scaleFactor) != FP_INFINITE);
    const auto inverseDivider = 1.0f / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider};
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
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

const float& Matrix3x2::operator()(std::size_t row, std::size_t column) const noexcept
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

float Matrix3x2::Determinant() const noexcept
{
    return (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
}

Matrix3x2 Matrix3x2::Multiply(
    const Matrix3x2& matrix1,
    const Matrix3x2& matrix2) noexcept
{
    return {
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1],
        matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix2.m[2][0],
        matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix2.m[2][1]};
}

Matrix3x2 Matrix3x2::Multiply(
    const Matrix3x2& matrix1, float scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor,
        matrix1.m[2][0] * scaleFactor,
        matrix1.m[2][1] * scaleFactor};
}

void Matrix3x2::Invert(const Matrix3x2& matrix, Matrix3x2& result)
{
    result = Invert(matrix);
}

Matrix3x2
Matrix3x2::Invert(const Matrix3x2& matrix)
{
    const auto determinant = matrix.Determinant();

    // NOTE: The 'matrix' must be non-singular matrix.
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_ZERO);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_NAN);
    POMDOG_ASSERT(std::fpclassify(determinant) != FP_INFINITE);

    const auto inverseDeterminant = 1.0f / determinant;
    const auto offsetX = matrix(2, 0);
    const auto offsetY = matrix(2, 1);

    return {
        matrix(1, 1) * inverseDeterminant,
        -matrix(0, 1) * inverseDeterminant,
        -matrix(1, 0) * inverseDeterminant,
        matrix(0, 0) * inverseDeterminant,
        (matrix(1, 0) * offsetY - offsetX * matrix(1, 1)) * inverseDeterminant,
        (offsetX * matrix(0, 1) - matrix(0, 0) * offsetY) * inverseDeterminant};
}

void Matrix3x2::CreateTranslation(const Vector2& position, Matrix3x2& result) noexcept
{
    result = Identity;
    result(2, 0) = position.X;
    result(2, 1) = position.Y;
}

Matrix3x2
Matrix3x2::CreateTranslation(const Vector2& position) noexcept
{
    Matrix3x2 result;
    CreateTranslation(position, result);
    return result;
}

void Matrix3x2::CreateScale(float scale, Matrix3x2& result) noexcept
{
    result.m[0][0] = scale;
    result.m[0][1] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

Matrix3x2
Matrix3x2::CreateScale(float scale) noexcept
{
    Matrix3x2 result;
    CreateScale(scale, result);
    return result;
}

void Matrix3x2::CreateScale(const Vector2& scale, Matrix3x2& result) noexcept
{
    result.m[0][0] = scale.X;
    result.m[0][1] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale.Y;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

Matrix3x2
Matrix3x2::CreateScale(const Vector2& scale) noexcept
{
    Matrix3x2 result;
    CreateScale(scale, result);
    return result;
}

void Matrix3x2::CreateRotation(const Radian<float>& angle, Matrix3x2& result)
{
    const auto sinAngle = std::sin(angle.value);
    const auto cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = sinAngle;
    result.m[1][0] = -sinAngle;
    result.m[1][1] = cosAngle;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

Matrix3x2
Matrix3x2::CreateRotation(const Radian<float>& angle)
{
    Matrix3x2 result;
    CreateRotation(angle, result);
    return result;
}

void Matrix3x2::CreateSkew(const Vector2& skew, Matrix3x2& result)
{
    result.m[0][0] = 1;
    result.m[0][1] = std::tan(skew.Y);
    result.m[1][0] = std::tan(skew.X);
    result.m[1][1] = 1;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

Matrix3x2
Matrix3x2::CreateSkew(const Vector2& skew)
{
    Matrix3x2 result;
    CreateSkew(skew, result);
    return result;
}

void Matrix3x2::Lerp(const Matrix3x2& source1,
    const Matrix3x2& source2, float amount, Matrix3x2& result) noexcept
{
    result.m[0][0] = math::Lerp(source1.m[0][0], source2.m[0][0], amount);
    result.m[0][1] = math::Lerp(source1.m[0][1], source2.m[0][1], amount);
    result.m[1][0] = math::Lerp(source1.m[1][0], source2.m[1][0], amount);
    result.m[1][1] = math::Lerp(source1.m[1][1], source2.m[1][1], amount);
    result.m[2][0] = math::Lerp(source1.m[2][0], source2.m[2][0], amount);
    result.m[2][1] = math::Lerp(source1.m[2][1], source2.m[2][1], amount);
}

Matrix3x2
Matrix3x2::Lerp(const Matrix3x2& source1,
    const Matrix3x2& source2, float amount) noexcept
{
    Matrix3x2 result;
    Lerp(source1, source2, amount, result);
    return result;
}

const float* Matrix3x2::Data() const noexcept
{
    static_assert(std::is_floating_point<float>::value, "T is floating point number");
    return m[0];
}

float* Matrix3x2::Data() noexcept
{
    static_assert(std::is_floating_point<float>::value, "T is floating point number");
    return m[0];
}

[[nodiscard]] Matrix3x2
operator*(float scaleFactor, const Matrix3x2& matrix) noexcept
{
    return Matrix3x2::Multiply(matrix, scaleFactor);
}

} // namespace pomdog
