// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointMatrix3x2.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix2x2.hpp"
#include "Pomdog/Math/detail/FloatingPointQuaternion.hpp"
#include "Pomdog/Math/detail/FloatingPointVector2.hpp"
#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cfloat>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointMatrix3x2<T>::FloatingPointMatrix3x2(
    T m00, T m01,
    T m10, T m11,
    T m20, T m21) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[1][0] = m10;
    m[1][1] = m11;
    m[2][0] = m20;
    m[2][1] = m21;
}

template <typename T>
FloatingPointMatrix3x2<T> & FloatingPointMatrix3x2<T>::operator*=(const FloatingPointMatrix3x2& other) noexcept
{
    *this = Multiply(*this, other);
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> & FloatingPointMatrix3x2<T>::operator+=(const FloatingPointMatrix3x2& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    m[2][0] += other.m[2][0];
    m[2][1] += other.m[2][1];
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> & FloatingPointMatrix3x2<T>::operator-=(const FloatingPointMatrix3x2& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    m[2][0] -= other.m[2][0];
    m[2][1] -= other.m[2][1];
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> & FloatingPointMatrix3x2<T>::operator*=(T scaleFactor) noexcept
{
    m[0][0] *= scaleFactor;
    m[0][1] *= scaleFactor;
    m[1][0] *= scaleFactor;
    m[1][1] *= scaleFactor;
    m[2][0] *= scaleFactor;
    m[2][1] *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> & FloatingPointMatrix3x2<T>::operator/=(T scaleFactor)
{
    auto const inverseDivider = T{1} / scaleFactor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    m[2][0] *= inverseDivider;
    m[2][1] *= inverseDivider;
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator-() const noexcept
{
    return Multiply(*this, T{-1});
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator+(const FloatingPointMatrix3x2& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1],
        m[2][0] + other.m[2][0],
        m[2][1] + other.m[2][1]};
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator-(const FloatingPointMatrix3x2& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1],
        m[2][0] - other.m[2][0],
        m[2][1] - other.m[2][1]};
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator*(const FloatingPointMatrix3x2& other) const noexcept
{
    return Multiply(*this, other);
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator*(T scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != T{0});
    auto const inverseDivider = T{1} / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider,
        m[2][0] * inverseDivider,
        m[2][1] * inverseDivider};
}

template <typename T>
bool FloatingPointMatrix3x2<T>::operator==(const FloatingPointMatrix3x2& other) const noexcept
{
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1]
        && m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1]
        && m[2][0] == other.m[2][0] && m[2][1] == other.m[2][1];
}

template <typename T>
bool FloatingPointMatrix3x2<T>::operator!=(const FloatingPointMatrix3x2& other) const noexcept
{
    return m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1]
        || m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1]
        || m[2][0] != other.m[2][0] || m[2][1] != other.m[2][1];
}

template <typename T>
T & FloatingPointMatrix3x2<T>::operator()(std::size_t row, std::size_t column)
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

template <typename T>
const T& FloatingPointMatrix3x2<T>::operator()(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

template <typename T>
T FloatingPointMatrix3x2<T>::Determinant() const noexcept
{
    return (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::Multiply(
    const FloatingPointMatrix3x2& matrix1,
    const FloatingPointMatrix3x2& matrix2) noexcept
{
    return {
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1],
        matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix2.m[2][0],
        matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix2.m[2][1]};
}

template <typename T>
FloatingPointMatrix3x2<T> FloatingPointMatrix3x2<T>::Multiply(
    const FloatingPointMatrix3x2& matrix1, T scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor,
        matrix1.m[2][0] * scaleFactor,
        matrix1.m[2][1] * scaleFactor};
}

template <typename T>
void
FloatingPointMatrix3x2<T>::Invert(const FloatingPointMatrix3x2& matrix, FloatingPointMatrix3x2 & result)
{
    result = Invert(matrix);
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::Invert(const FloatingPointMatrix3x2& matrix)
{
    auto const determinant = matrix.Determinant();

    static_assert(std::is_same<decltype(matrix.Determinant()), T>::value, "determinant is T");
    POMDOG_ASSERT_MESSAGE(T{0} != determinant, "This is singular matrix");

    auto const inverseDeterminant = T{1} / determinant;
    auto const offsetX = matrix(2, 0);
    auto const offsetY = matrix(2, 1);

    return {
         matrix(1, 1) * inverseDeterminant,
        -matrix(0, 1) * inverseDeterminant,
        -matrix(1, 0) * inverseDeterminant,
         matrix(0, 0) * inverseDeterminant,
        (matrix(1, 0) * offsetY - offsetX * matrix(1, 1)) * inverseDeterminant,
        (offsetX * matrix(0, 1) - matrix(0, 0) * offsetY) * inverseDeterminant};
}

template <typename T>
void
FloatingPointMatrix3x2<T>::CreateTranslation(const FloatingPointVector2<T>& position, FloatingPointMatrix3x2 & result) noexcept
{
    result = Identity;
    result(2, 0) = position.X;
    result(2, 1) = position.Y;
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::CreateTranslation(const FloatingPointVector2<T>& position) noexcept
{
    FloatingPointMatrix3x2 result;
    CreateTranslation(position, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix3x2<T>::CreateScale(T scale, FloatingPointMatrix3x2 & result) noexcept
{
    result.m[0][0] = scale;
    result.m[0][1] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::CreateScale(T scale) noexcept
{
    FloatingPointMatrix3x2 result;
    CreateScale(scale, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix3x2<T>::CreateScale(const FloatingPointVector2<T>& scale, FloatingPointMatrix3x2 & result) noexcept
{
    static_assert(std::is_same<T, decltype(scale.X)>::value, "scale is T");
    static_assert(std::is_same<T, decltype(scale.Y)>::value, "scale is T");

    result.m[0][0] = scale.X;
    result.m[0][1] = 0;
    result.m[1][0] = 0;
    result.m[1][1] = scale.Y;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::CreateScale(const FloatingPointVector2<T>& scale) noexcept
{
    FloatingPointMatrix3x2 result;
    CreateScale(scale, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix3x2<T>::CreateRotation(const Radian<T>& angle, FloatingPointMatrix3x2 & result)
{
    static_assert(std::is_same<T, decltype(angle.value)>::value, "angle is T");
    auto const sinAngle = std::sin(angle.value);
    auto const cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = sinAngle;
    result.m[1][0] = -sinAngle;
    result.m[1][1] = cosAngle;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::CreateRotation(const Radian<T>& angle)
{
    FloatingPointMatrix3x2 result;
    CreateRotation(angle, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix3x2<T>::CreateSkew(const FloatingPointVector2<T>& skew, FloatingPointMatrix3x2 & result)
{
    static_assert(std::is_same<T, decltype(skew.X)>::value, "scale is T");
    static_assert(std::is_same<T, decltype(skew.Y)>::value, "scale is T");

    result.m[0][0] = 1;
    result.m[0][1] = std::tan(skew.Y);
    result.m[1][0] = std::tan(skew.X);
    result.m[1][1] = 1;
    result.m[2][0] = 0;
    result.m[2][1] = 0;
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::CreateSkew(const FloatingPointVector2<T>& skew)
{
    FloatingPointMatrix3x2 result;
    CreateSkew(skew, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix3x2<T>::Lerp(const FloatingPointMatrix3x2& source1,
    const FloatingPointMatrix3x2& source2, T amount, FloatingPointMatrix3x2 & result) noexcept
{
    result.m[0][0] = MathHelper::Lerp(source1.m[0][0], source2.m[0][0], amount);
    result.m[0][1] = MathHelper::Lerp(source1.m[0][1], source2.m[0][1], amount);
    result.m[1][0] = MathHelper::Lerp(source1.m[1][0], source2.m[1][0], amount);
    result.m[1][1] = MathHelper::Lerp(source1.m[1][1], source2.m[1][1], amount);
    result.m[2][0] = MathHelper::Lerp(source1.m[2][0], source2.m[2][0], amount);
    result.m[2][1] = MathHelper::Lerp(source1.m[2][1], source2.m[2][1], amount);
}

template <typename T>
FloatingPointMatrix3x2<T>
FloatingPointMatrix3x2<T>::Lerp(const FloatingPointMatrix3x2& source1,
    const FloatingPointMatrix3x2& source2, T amount) noexcept
{
    FloatingPointMatrix3x2 result;
    Lerp(source1, source2, amount, result);
    return std::move(result);
}

template <typename T>
const T* FloatingPointMatrix3x2<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
T* FloatingPointMatrix3x2<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
FloatingPointMatrix3x2<T> operator*(T scaleFactor, const FloatingPointMatrix3x2<T>& matrix) noexcept
{
    return FloatingPointMatrix3x2<T>::Multiply(matrix, scaleFactor);
}

// explicit instantiations
template class FloatingPointMatrix3x2<float>;

template FloatingPointMatrix3x2<float>
operator*<float>(float, const FloatingPointMatrix3x2<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointMatrix3x2<double>;

template FloatingPointMatrix3x2<double>
operator*<double>(double, const FloatingPointMatrix3x2<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointMatrix3x2<long double>;

template FloatingPointMatrix3x2<long double>
operator*<long double>(long double, const FloatingPointMatrix3x2<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
