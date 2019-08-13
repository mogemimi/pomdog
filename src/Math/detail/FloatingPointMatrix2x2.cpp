// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointMatrix2x2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cfloat>
#include <cmath>
#include <limits>

namespace Pomdog::Detail {

template <typename T>
FloatingPointMatrix2x2<T>::FloatingPointMatrix2x2(T m00, T m01, T m10, T m11) noexcept
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[1][0] = m10;
    m[1][1] = m11;
}

template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator*=(const FloatingPointMatrix2x2& other) noexcept
{
    *this = Multiply(*this, other);
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator+=(const FloatingPointMatrix2x2& other) noexcept
{
    m[0][0] += other.m[0][0];
    m[0][1] += other.m[0][1];
    m[1][0] += other.m[1][0];
    m[1][1] += other.m[1][1];
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator-=(const FloatingPointMatrix2x2& other) noexcept
{
    m[0][0] -= other.m[0][0];
    m[0][1] -= other.m[0][1];
    m[1][0] -= other.m[1][0];
    m[1][1] -= other.m[1][1];
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator*=(T scaleFactor) noexcept
{
    m[0][0] *= scaleFactor;
    m[0][1] *= scaleFactor;
    m[1][0] *= scaleFactor;
    m[1][1] *= scaleFactor;
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> &
FloatingPointMatrix2x2<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != T{0});
    auto const inverseDivider = T{1} / scaleFactor;
    m[0][0] *= inverseDivider;
    m[0][1] *= inverseDivider;
    m[1][0] *= inverseDivider;
    m[1][1] *= inverseDivider;
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator-() const noexcept
{
    return {-m[0][0], -m[0][1], -m[1][0], -m[1][1]};
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator+(const FloatingPointMatrix2x2& other) const noexcept
{
    return {
        m[0][0] + other.m[0][0],
        m[0][1] + other.m[0][1],
        m[1][0] + other.m[1][0],
        m[1][1] + other.m[1][1]};
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator-(const FloatingPointMatrix2x2& other) const noexcept
{
    return {
        m[0][0] - other.m[0][0],
        m[0][1] - other.m[0][1],
        m[1][0] - other.m[1][0],
        m[1][1] - other.m[1][1]};
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator*(const FloatingPointMatrix2x2& other) const noexcept
{
    return Multiply(*this, other);
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator*(T scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != T{0});
    auto const inverseDivider = T{1} / scaleFactor;
    return {
        m[0][0] * inverseDivider,
        m[0][1] * inverseDivider,
        m[1][0] * inverseDivider,
        m[1][1] * inverseDivider};
}

template <typename T>
bool FloatingPointMatrix2x2<T>::operator==(const FloatingPointMatrix2x2& other) const noexcept
{
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1]
        && m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1];
}

template <typename T>
bool FloatingPointMatrix2x2<T>::operator!=(const FloatingPointMatrix2x2& other) const noexcept
{
    return m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1]
        || m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1];
}

template <typename T>
T & FloatingPointMatrix2x2<T>::operator()(std::size_t row, std::size_t column)
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

template <typename T>
const T& FloatingPointMatrix2x2<T>::operator()(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::Multiply(
    const FloatingPointMatrix2x2& matrix1,
    const FloatingPointMatrix2x2& matrix2) noexcept
{
    return {
        matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0],
        matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1],
        matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0],
        matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1]};
}

template <typename T>
FloatingPointMatrix2x2<T> FloatingPointMatrix2x2<T>::Multiply(
    const FloatingPointMatrix2x2& matrix1, T scaleFactor) noexcept
{
    return {
        matrix1.m[0][0] * scaleFactor,
        matrix1.m[0][1] * scaleFactor,
        matrix1.m[1][0] * scaleFactor,
        matrix1.m[1][1] * scaleFactor};
}

template <typename T>
T FloatingPointMatrix2x2<T>::Determinant() const noexcept
{
    return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

template <typename T>
const T* FloatingPointMatrix2x2<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
T* FloatingPointMatrix2x2<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
FloatingPointMatrix2x2<T> operator*(T scaleFactor, const FloatingPointMatrix2x2<T>& matrix) noexcept
{
    return FloatingPointMatrix2x2<T>::Multiply(matrix, scaleFactor);
}

// explicit instantiations
template class FloatingPointMatrix2x2<float>;

template FloatingPointMatrix2x2<float>
operator*<float>(float, const FloatingPointMatrix2x2<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointMatrix2x2<double>;

template FloatingPointMatrix2x2<double>
operator*<double>(double, const FloatingPointMatrix2x2<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointMatrix2x2<long double>;

template FloatingPointMatrix2x2<long double>
operator*<long double>(long double, const FloatingPointMatrix2x2<long double>&) noexcept;
#endif

} // namespace Pomdog::Detail
