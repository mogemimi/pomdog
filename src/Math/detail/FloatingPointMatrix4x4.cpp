// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix3x3.hpp"
#include "Pomdog/Math/detail/FloatingPointQuaternion.hpp"
#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cfloat>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

template <typename T>
FloatingPointMatrix4x4<T>::FloatingPointMatrix4x4(
    T m00, T m01, T m02, T m03,
    T m10, T m11, T m12, T m13,
    T m20, T m21, T m22, T m23,
    T m30, T m31, T m32, T m33) noexcept
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

template <typename T>
const T& FloatingPointMatrix4x4<T>::operator()(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

template <typename T>
T & FloatingPointMatrix4x4<T>::operator()(std::size_t row, std::size_t column)
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");
    return m[row][column];
}

//template <typename T>
//const std::array<T, 4>& FloatingPointMatrix4x4<T>::operator[](std::size_t row) const
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "FloatingPointMatrix4x4, subscript out of range");
//    return m[row];
//}

//template <typename T>
//std::array<T, 4> & FloatingPointMatrix4x4<T>::operator[](std::size_t row)
//{
//    POMDOG_ASSERT_MESSAGE(row < RowSize, "FloatingPointMatrix4x4, subscript out of range");
//    return m[row];
//}

template <typename T>
FloatingPointMatrix4x4<T> & FloatingPointMatrix4x4<T>::operator*=(const FloatingPointMatrix4x4& other) noexcept
{
    *this = Multiply(*this, other);
    return *this;
}

template <typename T>
FloatingPointMatrix4x4<T> & FloatingPointMatrix4x4<T>::operator+=(const FloatingPointMatrix4x4& other) noexcept
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

template <typename T>
FloatingPointMatrix4x4<T> & FloatingPointMatrix4x4<T>::operator-=(const FloatingPointMatrix4x4& other) noexcept
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

template <typename T>
FloatingPointMatrix4x4<T> & FloatingPointMatrix4x4<T>::operator*=(T scaleFactor) noexcept
{
    *this = Multiply(*this, scaleFactor);
    return *this;
}

template <typename T>
FloatingPointMatrix4x4<T> & FloatingPointMatrix4x4<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    auto const inverseDivider = 1 / scaleFactor;
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

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator+() const noexcept
{
    return *this;
}

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator-() const noexcept
{
    return Multiply(*this, -1);
}

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator+(const FloatingPointMatrix4x4& other) const noexcept
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

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator-(const FloatingPointMatrix4x4& other) const noexcept
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

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator*(const FloatingPointMatrix4x4& other) const noexcept
{
    return Multiply(*this, other);
}

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator*(T scaleFactor) const noexcept
{
    return Multiply(*this, scaleFactor);
}

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != T{0});
    auto const inverseDivider = T{1} / scaleFactor;
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

template <typename T>
bool FloatingPointMatrix4x4<T>::operator==(const FloatingPointMatrix4x4& other) const noexcept
{
    return m[0][0] == other.m[0][0] && m[0][1] == other.m[0][1] && m[0][2] == other.m[0][2] && m[0][3] == other.m[0][3]
        && m[1][0] == other.m[1][0] && m[1][1] == other.m[1][1] && m[1][2] == other.m[1][2] && m[1][3] == other.m[1][3]
        && m[2][0] == other.m[2][0] && m[2][1] == other.m[2][1] && m[2][2] == other.m[2][2] && m[2][3] == other.m[2][3]
        && m[3][0] == other.m[3][0] && m[3][1] == other.m[3][1] && m[3][2] == other.m[3][2] && m[3][3] == other.m[3][3];
}

template <typename T>
bool FloatingPointMatrix4x4<T>::operator!=(const FloatingPointMatrix4x4& other) const noexcept
{
    return m[0][0] != other.m[0][0] || m[0][1] != other.m[0][1] || m[0][2] != other.m[0][2] || m[0][3] != other.m[0][3]
        || m[1][0] != other.m[1][0] || m[1][1] != other.m[1][1] || m[1][2] != other.m[1][2] || m[1][3] != other.m[1][3]
        || m[2][0] != other.m[2][0] || m[2][1] != other.m[2][1] || m[2][2] != other.m[2][2] || m[2][3] != other.m[2][3]
        || m[3][0] != other.m[3][0] || m[3][1] != other.m[3][1] || m[3][2] != other.m[3][2] || m[3][3] != other.m[3][3];
}

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::Multiply(
    const FloatingPointMatrix4x4& matrix1,
    const FloatingPointMatrix4x4& matrix2) noexcept
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

template <typename T>
FloatingPointMatrix4x4<T> FloatingPointMatrix4x4<T>::Multiply(
    const FloatingPointMatrix4x4& matrix1, T scaleFactor) noexcept
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

template <typename T>
void FloatingPointMatrix4x4<T>::SetTranslation(const FloatingPointVector3<T>& position) noexcept
{
    m[3][0] = position.X;
    m[3][1] = position.Y;
    m[3][2] = position.Z;
}

template <typename T>
FloatingPointVector3<T> FloatingPointMatrix4x4<T>::GetTranslation() const noexcept
{
    return {m[3][0], m[3][1], m[3][2]};
}

template <typename T>
void FloatingPointMatrix4x4<T>::SetScale(const FloatingPointVector3<T>& scale) noexcept
{
    m[0][0] = scale.X;
    m[1][1] = scale.Y;
    m[2][2] = scale.Z;
}

template <typename T>
FloatingPointVector3<T> FloatingPointMatrix4x4<T>::GetScale() const noexcept
{
    return {m[0][0], m[1][1], m[2][2]};
}

template <typename T>
T FloatingPointMatrix4x4<T>::Determinant() const noexcept
{
    // TODO: Replace with simple process using cofactors to compute a determinant
    // please see: FloatingPointMatrix3x3::Determinant implementation
    return
    + m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][0] * m[1][3] * m[2][1] * m[3][2]
    + m[0][1] * m[1][0] * m[2][3] * m[3][2] + m[0][1] * m[1][2] * m[2][0] * m[3][3] + m[0][1] * m[1][3] * m[2][2] * m[3][0]
    + m[0][2] * m[1][0] * m[2][1] * m[3][3] + m[0][2] * m[1][1] * m[2][3] * m[3][0] + m[0][2] * m[1][3] * m[2][0] * m[3][1]
    + m[0][3] * m[1][0] * m[2][2] * m[3][1] + m[0][3] * m[1][1] * m[2][0] * m[3][2] + m[0][3] * m[1][2] * m[2][1] * m[3][0]
    - m[0][0] * m[1][1] * m[2][3] * m[3][2] - m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][0] * m[1][3] * m[2][2] * m[3][1]
    - m[0][1] * m[1][0] * m[2][2] * m[3][3] - m[0][1] * m[1][2] * m[2][3] * m[3][0] - m[0][1] * m[1][3] * m[2][0] * m[3][2]
    - m[0][2] * m[1][0] * m[2][3] * m[3][1] - m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][2] * m[1][3] * m[2][1] * m[3][0]
    - m[0][3] * m[1][0] * m[2][1] * m[3][2] - m[0][3] * m[1][1] * m[2][2] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1];
}

template <typename T>
FloatingPointMatrix3x3<T> FloatingPointMatrix4x4<T>::Minor3x3(std::size_t row, std::size_t column) const
{
    POMDOG_ASSERT_MESSAGE(row < RowSize, "row: out of range");
    POMDOG_ASSERT_MESSAGE(column < ColumnSize, "column: out of range");

    //      4x4 matrix                              3x3 minor
    //     c0  c1  c2  c3
    // r0 |11, 12, 13, 14|                       |11, 12, 13, x|
    // r1 |21, 22, 23, 24| Minor3x3(mat, r2, c3) |21, 22. 23, x|
    // r2 |31, 32, 33, 34| --------------------> |41, 42, 43, x|
    // r3 |41, 42, 43, 44|                       | x,  x,  x, x|

    FloatingPointMatrix3x3<T> minorMatrix;
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
    return std::move(minorMatrix);
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::Adjoint(const FloatingPointMatrix4x4& matrix)
{
    return {
         matrix.Minor3x3(0, 0).Determinant(),
        -matrix.Minor3x3(1, 0).Determinant(),
         matrix.Minor3x3(2, 0).Determinant(),
        -matrix.Minor3x3(3, 0).Determinant(),
        -matrix.Minor3x3(0, 1).Determinant(),
         matrix.Minor3x3(1, 1).Determinant(),
        -matrix.Minor3x3(2, 1).Determinant(),
         matrix.Minor3x3(3, 1).Determinant(),
         matrix.Minor3x3(0, 2).Determinant(),
        -matrix.Minor3x3(1, 2).Determinant(),
         matrix.Minor3x3(2, 2).Determinant(),
        -matrix.Minor3x3(3, 2).Determinant(),
        -matrix.Minor3x3(0, 3).Determinant(),
         matrix.Minor3x3(1, 3).Determinant(),
        -matrix.Minor3x3(2, 3).Determinant(),
         matrix.Minor3x3(3, 3).Determinant()};
}

template <typename T>
void
FloatingPointMatrix4x4<T>::Transpose(const FloatingPointMatrix4x4& matrix, FloatingPointMatrix4x4 & result) noexcept
{
    result = {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]};
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::Transpose(const FloatingPointMatrix4x4& matrix) noexcept
{
    return {
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]};
}

template <typename T>
void
FloatingPointMatrix4x4<T>::Invert(const FloatingPointMatrix4x4& matrix, FloatingPointMatrix4x4 & result)
{
    auto const determinant = matrix.Determinant();
    POMDOG_ASSERT_MESSAGE(0 != determinant, "This is singular matrix");
    result = Adjoint(matrix) / determinant;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::Invert(const FloatingPointMatrix4x4& matrix)
{
    FloatingPointMatrix4x4 result;
    Invert(matrix, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::Lerp(const FloatingPointMatrix4x4& source1, const FloatingPointMatrix4x4& source2,
    T amount, FloatingPointMatrix4x4 & result) noexcept
{
    result.m[0][0] = MathHelper::Lerp(source1.m[0][0], source2.m[0][0], amount);
    result.m[0][1] = MathHelper::Lerp(source1.m[0][1], source2.m[0][1], amount);
    result.m[0][2] = MathHelper::Lerp(source1.m[0][2], source2.m[0][2], amount);
    result.m[0][3] = MathHelper::Lerp(source1.m[0][3], source2.m[0][3], amount);
    result.m[1][0] = MathHelper::Lerp(source1.m[1][0], source2.m[1][0], amount);
    result.m[1][1] = MathHelper::Lerp(source1.m[1][1], source2.m[1][1], amount);
    result.m[1][2] = MathHelper::Lerp(source1.m[1][2], source2.m[1][2], amount);
    result.m[1][3] = MathHelper::Lerp(source1.m[1][3], source2.m[1][3], amount);
    result.m[2][0] = MathHelper::Lerp(source1.m[2][0], source2.m[2][0], amount);
    result.m[2][1] = MathHelper::Lerp(source1.m[2][1], source2.m[2][1], amount);
    result.m[2][2] = MathHelper::Lerp(source1.m[2][2], source2.m[2][2], amount);
    result.m[2][3] = MathHelper::Lerp(source1.m[2][3], source2.m[2][3], amount);
    result.m[3][0] = MathHelper::Lerp(source1.m[3][0], source2.m[3][0], amount);
    result.m[3][1] = MathHelper::Lerp(source1.m[3][1], source2.m[3][1], amount);
    result.m[3][2] = MathHelper::Lerp(source1.m[3][2], source2.m[3][2], amount);
    result.m[3][3] = MathHelper::Lerp(source1.m[3][3], source2.m[3][3], amount);
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::Lerp(const FloatingPointMatrix4x4& source1, const FloatingPointMatrix4x4& source2,
    T amount) noexcept
{
    FloatingPointMatrix4x4 result;
    Lerp(source1, source2, amount, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateTranslation(const FloatingPointVector3<T>& position, FloatingPointMatrix4x4 & result) noexcept
{
    result = Identity;
    result.m[3][0] = position.X;
    result.m[3][1] = position.Y;
    result.m[3][2] = position.Z;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateTranslation(const FloatingPointVector3<T>& position) noexcept
{
    FloatingPointMatrix4x4 result;
    CreateTranslation(position, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateScale(T scale, FloatingPointMatrix4x4 & result) noexcept
{
    result = Identity;
    result.m[0][0] = scale;
    result.m[1][1] = scale;
    result.m[2][2] = scale;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateScale(T scale) noexcept
{
    FloatingPointMatrix4x4 result;
    CreateScale(scale, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateScale(const FloatingPointVector3<T>& scale, FloatingPointMatrix4x4 & result) noexcept
{
    result = Identity;
    result.m[0][0] = scale.X;
    result.m[1][1] = scale.Y;
    result.m[2][2] = scale.Z;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateScale(const FloatingPointVector3<T>& scale) noexcept
{
    FloatingPointMatrix4x4 result;
    CreateScale(scale, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateRotationX(const Radian<T>& angle, FloatingPointMatrix4x4 & result)
{
    result = Identity;
    auto const sinAngle = std::sin(angle.value);
    auto const cosAngle = std::cos(angle.value);

    result.m[1][1] = cosAngle;
    result.m[1][2] = sinAngle;
    result.m[2][1] = -sinAngle;
    result.m[2][2] = cosAngle;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateRotationX(const Radian<T>& angle)
{
    FloatingPointMatrix4x4 result;
    CreateRotationX(angle, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateRotationY(const Radian<T>& angle, FloatingPointMatrix4x4 & result)
{
    result = Identity;
    auto const sinAngle = std::sin(angle.value);
    auto const cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][2] = -sinAngle;
    result.m[2][0] = sinAngle;
    result.m[2][2] = cosAngle;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateRotationY(const Radian<T>& angle)
{
    FloatingPointMatrix4x4 result;
    CreateRotationY(angle, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateRotationZ(const Radian<T>& angle, FloatingPointMatrix4x4 & result)
{
    result = Identity;
    auto const sinAngle = std::sin(angle.value);
    auto const cosAngle = std::cos(angle.value);

    result.m[0][0] = cosAngle;
    result.m[0][1] = sinAngle;
    result.m[1][0] = -sinAngle;
    result.m[1][1] = cosAngle;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateRotationZ(const Radian<T>& angle)
{
    FloatingPointMatrix4x4 result;
    CreateRotationZ(angle, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateFromQuaternion(const FloatingPointQuaternion<T>& quaternion, FloatingPointMatrix4x4 & result)
{
    auto const xx = quaternion.X * quaternion.X;
    auto const yy = quaternion.Y * quaternion.Y;
    auto const zz = quaternion.Z * quaternion.Z;
    auto const xy = quaternion.X * quaternion.Y;
    auto const zw = quaternion.Z * quaternion.W;
    auto const zx = quaternion.Z * quaternion.X;
    auto const yw = quaternion.Y * quaternion.W;
    auto const yz = quaternion.Y * quaternion.Z;
    auto const xw = quaternion.X * quaternion.W;

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateFromQuaternion(const FloatingPointQuaternion<T>& quaternion)
{
    FloatingPointMatrix4x4 result;
    CreateFromQuaternion(quaternion, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateLookAtLH(
    const FloatingPointVector3<T>& eye,
    const FloatingPointVector3<T>& at,
    const FloatingPointVector3<T>& up,
    FloatingPointMatrix4x4 & result)
{
    // zaxis = normal(At - Eye)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    auto const zaxis = FloatingPointVector3<T>::Normalize(at - eye);// RH: eye - at
    auto const xaxis = FloatingPointVector3<T>::Normalize(FloatingPointVector3<T>::Cross(up, zaxis));
    auto const yaxis = FloatingPointVector3<T>::Cross(zaxis, xaxis);

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

    result.m[3][0] = -FloatingPointVector3<T>::Dot(xaxis, eye);
    result.m[3][1] = -FloatingPointVector3<T>::Dot(yaxis, eye);
    result.m[3][2] = -FloatingPointVector3<T>::Dot(zaxis, eye);
    result.m[3][3] = 1;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateLookAtLH(const FloatingPointVector3<T>& eye, const FloatingPointVector3<T>& at,
    const FloatingPointVector3<T>& up)
{// Left-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreateLookAtLH(eye, at, up, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateLookAtRH(const FloatingPointVector3<T>& eye, const FloatingPointVector3<T>& at,
    const FloatingPointVector3<T>& up, FloatingPointMatrix4x4 & result)
{
    // zaxis = normal(Eye - At)
    // xaxis = normal(cross(Up, zaxis))
    // yaxis = cross(zaxis, xaxis)
    //
    //  xaxis.x           yaxis.x           zaxis.x          0
    //  xaxis.y           yaxis.y           zaxis.y          0
    //  xaxis.z           yaxis.z           zaxis.z          0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1

    auto const zaxis = FloatingPointVector3<T>::Normalize(eye - at);// LH: at - eye
    auto const xaxis = FloatingPointVector3<T>::Normalize(FloatingPointVector3<T>::Cross(up, zaxis));
    auto const yaxis = FloatingPointVector3<T>::Cross(zaxis, xaxis);

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

    result.m[3][0] = -FloatingPointVector3<T>::Dot(xaxis, eye);
    result.m[3][1] = -FloatingPointVector3<T>::Dot(yaxis, eye);
    result.m[3][2] = -FloatingPointVector3<T>::Dot(zaxis, eye);
    result.m[3][3] = 1;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateLookAtRH(const FloatingPointVector3<T>& eye, const FloatingPointVector3<T>& at,
    const FloatingPointVector3<T>& up)
{// Right-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    FloatingPointMatrix4x4::CreateLookAtRH(eye, at, up, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveLH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(width > 0,   "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0,  "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0,  "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0,    "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar,"zNear: out of range");

    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zf-zn)     1
    // 0       0       zn*zf/(zn-zf)  0

    T const a = 2 * zNear / width;
    T const b = 2 * zNear / height;
    T const c = zFar / (zFar - zNear);// RH: c = zFar / (zNear - zFar);
    T const d = zNear * zFar / (zNear - zFar);

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
    result.m[2][3] = 1;// RH: -1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveRH(T width, T height, T zNear, T zFar, FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(width > 0,  "width: out of range");
    POMDOG_ASSERT_MESSAGE(height > 0, "height: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0,     "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zn-zf)    -1
    // 0       0       zn*zf/(zn-zf)  0

    T const a = 2.0f * zNear / width;
    T const b = 2.0f * zNear / height;
    T const c = zFar / (zNear - zFar);//LH: c = zFar / (zFar - zNear);
    T const d = zNear * zFar / (zNear - zFar);

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
    result.m[2][3] = -1;// LH: +1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveFieldOfViewLH(const Radian<T>& fovy, T aspect, T zNear, T zFar,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0,  "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0,  "zFar: out of range");
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

    T const yScale = T{1} / std::tan(fovy.value / 2);
    T const xScale = yScale / aspect;
    T const p = zFar / (zFar - zNear);

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreatePerspectiveFieldOfViewLH(const Radian<T>& fovy, T aspect, T zNear, T zFar)
{// Left-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreatePerspectiveFieldOfViewLH(fovy, aspect, zNear, zFar, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveFieldOfViewRH(const Radian<T>& fovy, T aspect, T zNear, T zFar,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(fovy > 0, "fovy: out of range");
    POMDOG_ASSERT_MESSAGE(aspect > 0, "aspect: out of range");
    POMDOG_ASSERT_MESSAGE(zNear > 0,  "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0,    "zFar: out of range");
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

    T const yScale = 1 / std::tan(fovy.value / 2);
    T const xScale = yScale / aspect;
    T const p = zFar / (zNear - zFar);

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreatePerspectiveFieldOfViewRH(const Radian<T>& fovy, T aspect, T zNear, T zFar)
{// Right-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreatePerspectiveFieldOfViewRH(fovy, aspect, zNear, zFar, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
    // 0            0            zn*zf/(zn-zf)  0

    T const x = 2 * zNear / (right - left);
    T const y = 2 * zNear / (top - bottom);
    T const a = (left + right) / (left - right);// RH: (left + right) / (right - left)
    T const b = (top + bottom) / (bottom - top);// RH: (top + bottom) / (top - bottom)
    T const c = zFar / (zFar - zNear);// RH: zFar / (zNear - zFar)
    T const d = zNear * zFar / (zNear - zFar);

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
    result.m[2][3] = 1;// RH: -1

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = d;
    result.m[3][3] = 0;
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreatePerspectiveOffCenterLH(T left, T right, T bottom, T top, T zNear, T zFar)
{// Left-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreatePerspectiveOffCenterLH(left, right, bottom, top, zNear, zFar, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT_MESSAGE(zNear > 0, "zNear: out of range");
    POMDOG_ASSERT_MESSAGE(zFar > 0, "zFar: out of range");
    POMDOG_ASSERT_MESSAGE(zNear < zFar, "zNear: out of range");

    // 2*zn/(r-l)   0            0              0
    // 0            2*zn/(t-b)   0              0
    // (l+r)/(r-l)  (t+b)/(t-b)  zf/(zn-zf)    -1
    // 0            0            zn*zf/(zn-zf)  0

    T const x = 2 * zNear / (right - left);
    T const y = 2 * zNear / (top - bottom);
    T const a = (left + right) / (right - left);// LH: (left + right) / (left - right)
    T const b = (top + bottom) / (top - bottom);// LH: (top + bottom) / (bottom - top)
    T const c = zFar / (zNear - zFar);// LH: zFar / (zFar - zNear)
    T const d = zNear * zFar / (zNear - zFar);

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreatePerspectiveOffCenterRH(T left, T right, T bottom, T top, T zNear, T zFar)
{// Right-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreatePerspectiveOffCenterRH(left, right, bottom, top, zNear, zFar, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE:
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zf-zn)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    T const inverseRightLeft = 1 / (right - left);
    T const inverseTopBottom = 1 / (top - bottom);
    T const inverseFarNear   = 1 / (zFarPlane - zNearPlane);

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateOrthographicOffCenterLH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane)
{// Left-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreateOrthographicOffCenterLH(left, right, bottom, top, zNearPlane, zFarPlane, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane,
    FloatingPointMatrix4x4 & result)
{
    POMDOG_ASSERT((right - left) != 0);
    POMDOG_ASSERT((top - bottom) != 0);
    POMDOG_ASSERT(zNearPlane < zFarPlane);

    // NOTE:
    // 2/(r-l)      0            0           0
    // 0            2/(t-b)      0           0
    // 0            0            1/(zn-zf)   0
    // (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1

    T const inverseRightLeft = 1 / (right - left);
    T const inverseTopBottom = 1 / (top - bottom);
    T const inverseFarNear   = 1 / (zNearPlane - zFarPlane);

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateOrthographicOffCenterRH(T left, T right, T bottom, T top, T zNearPlane, T zFarPlane)
{// Right-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreateOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane, FloatingPointMatrix4x4 & result)
{
    CreateOrthographicOffCenterLH(
        -width / 2, width / 2, -height / 2, height / 2, zNearPlane, zFarPlane, result);
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateOrthographicLH(T width, T height, T zNearPlane, T zFarPlane)
{// Left-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreateOrthographicLH(width, height, zNearPlane, zFarPlane, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane, FloatingPointMatrix4x4 & result)
{
    CreateOrthographicOffCenterRH(
        -width / 2, width / 2, -height / 2, height / 2, zNearPlane, zFarPlane, result);
}

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateOrthographicRH(T width, T height, T zNearPlane, T zFarPlane)
{// Right-handed Cartesian Coordinates
    FloatingPointMatrix4x4 result;
    CreateOrthographicRH(width, height, zNearPlane, zFarPlane, result);
    return std::move(result);
}

template <typename T>
void
FloatingPointMatrix4x4<T>::CreateFromAxisAngle(
    const FloatingPointVector3<T>& axis, const Radian<T>& angle, FloatingPointMatrix4x4 & result)
{
    auto const sinAngle = std::sin(angle.value);
    auto const cosAngle = std::cos(angle.value);
    auto const scaleFactor = T{1} - cosAngle;
    auto const xx = axis.X * axis.X;
    auto const yy = axis.Y * axis.Y;
    auto const zz = axis.Z * axis.Z;
    auto const xy = axis.X * axis.Y;
    auto const xz = axis.X * axis.Z;
    auto const yz = axis.Y * axis.Z;

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

template <typename T>
FloatingPointMatrix4x4<T>
FloatingPointMatrix4x4<T>::CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle)
{
    FloatingPointMatrix4x4 result;
    CreateFromAxisAngle(axis, angle, result);
    return std::move(result);
}

template <typename T>
const T* FloatingPointMatrix4x4<T>::Data() const noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
T* FloatingPointMatrix4x4<T>::Data() noexcept
{
    static_assert(std::is_floating_point<T>::value, "T is floating point number");
    return m[0];
}

template <typename T>
FloatingPointMatrix4x4<T> operator*(T scaleFactor, const FloatingPointMatrix4x4<T>& matrix) noexcept
{
    return FloatingPointMatrix4x4<T>::Multiply(matrix, scaleFactor);
}

// explicit instantiations
template class FloatingPointMatrix4x4<float>;

template FloatingPointMatrix4x4<float>
operator*<float>(float, const FloatingPointMatrix4x4<float>&) noexcept;

#if defined(DBL_MANT_DIG)
template class FloatingPointMatrix4x4<double>;

template FloatingPointMatrix4x4<double>
operator*<double>(double, const FloatingPointMatrix4x4<double>&) noexcept;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointMatrix4x4<long double>;

template FloatingPointMatrix4x4<long double>
operator*<long double>(long double, const FloatingPointMatrix4x4<long double>&) noexcept;
#endif

} // namespace Detail
} // namespace Pomdog
