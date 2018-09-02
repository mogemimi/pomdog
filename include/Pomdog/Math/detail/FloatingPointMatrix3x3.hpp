// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointMatrix3x3 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T m[3][3];

private:
    static constexpr std::size_t RowSize = 3;
    static constexpr std::size_t ColumnSize = 3;

public:
    // Constructors:
    FloatingPointMatrix3x3() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointMatrix3x3(
        T m00, T m01, T m02,
        T m10, T m11, T m12,
        T m20, T m21, T m22) noexcept;

    // Assignment operators:
    FloatingPointMatrix3x3& operator*=(const FloatingPointMatrix3x3& other) noexcept;
    FloatingPointMatrix3x3& operator+=(const FloatingPointMatrix3x3& other) noexcept;
    FloatingPointMatrix3x3& operator-=(const FloatingPointMatrix3x3& other) noexcept;
    FloatingPointMatrix3x3& operator*=(T scaleFactor) noexcept;
    FloatingPointMatrix3x3& operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix3x3 operator+() const noexcept;
    FloatingPointMatrix3x3 operator-() const noexcept;

    // Binary operators:
    FloatingPointMatrix3x3 operator+(const FloatingPointMatrix3x3& other) const noexcept;
    FloatingPointMatrix3x3 operator-(const FloatingPointMatrix3x3& other) const noexcept;
    FloatingPointMatrix3x3 operator*(const FloatingPointMatrix3x3& other) const noexcept;
    FloatingPointMatrix3x3 operator*(T scaleFactor) const noexcept;
    FloatingPointMatrix3x3 operator/(T scaleFactor) const;

    bool operator==(const FloatingPointMatrix3x3& other) const noexcept;
    bool operator!=(const FloatingPointMatrix3x3& other) const noexcept;

    // Function-call operators:
    const T& operator()(std::size_t row, std::size_t column) const;
    T& operator()(std::size_t row, std::size_t column);

    void SetScale(const FloatingPointVector3<T>& scale) noexcept;

    FloatingPointVector3<T> GetScale() const noexcept;

    T Determinant() const noexcept;

    FloatingPointMatrix2x2<T> Minor2x2(std::size_t row, std::size_t column) const;

    static FloatingPointMatrix3x3
    Multiply(const FloatingPointMatrix3x3& matrix1, const FloatingPointMatrix3x3& matrix2) noexcept;

    static FloatingPointMatrix3x3
    Multiply(const FloatingPointMatrix3x3& matrix1, T scaleFactor) noexcept;

    static FloatingPointMatrix3x3
    Adjoint(const FloatingPointMatrix3x3& matrix);

    static void
    Transpose(const FloatingPointMatrix3x3& matrix, FloatingPointMatrix3x3& result) noexcept;

    static FloatingPointMatrix3x3
    Transpose(const FloatingPointMatrix3x3& matrix) noexcept;

    static void
    Invert(const FloatingPointMatrix3x3& matrix, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    Invert(const FloatingPointMatrix3x3& matrix);

    static void
    Lerp(const FloatingPointMatrix3x3& source1, const FloatingPointMatrix3x3& source2,
        T amount, FloatingPointMatrix3x3& result) noexcept;

    static FloatingPointMatrix3x3
    Lerp(const FloatingPointMatrix3x3& source1, const FloatingPointMatrix3x3& source2,
        T amount) noexcept;

    static void
    CreateTranslation(const FloatingPointVector2<T>& position, FloatingPointMatrix3x3& result) noexcept;

    static FloatingPointMatrix3x3
    CreateTranslation(const FloatingPointVector2<T>& position) noexcept;

    static void
    CreateScale(T scale, FloatingPointMatrix3x3& result) noexcept;

    static FloatingPointMatrix3x3
    CreateScale(T scale) noexcept;

    static void
    CreateScale(const FloatingPointVector3<T>& scale, FloatingPointMatrix3x3& result) noexcept;

    static FloatingPointMatrix3x3
    CreateScale(const FloatingPointVector3<T>& scale) noexcept;

    static void
    CreateRotationX(const Radian<T>& angle, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    CreateRotationX(const Radian<T>& angle);

    static void
    CreateRotationY(const Radian<T>& angle, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    CreateRotationY(const Radian<T>& angle);

    static void
    CreateRotationZ(const Radian<T>& angle, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    CreateRotationZ(const Radian<T>& angle);

    static void
    CreateFromQuaternion(const FloatingPointQuaternion<T>& quaternion, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    CreateFromQuaternion(const FloatingPointQuaternion<T>& quaternion);

    static void
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle, FloatingPointMatrix3x3& result);

    static FloatingPointMatrix3x3
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle);

    ///@brief Returns pointer to the first element.
    const T* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointMatrix3x3 const Identity;
};

template <typename T>
FloatingPointMatrix3x3<T> const FloatingPointMatrix3x3<T>::Identity = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
};

template <typename T>
FloatingPointMatrix3x3<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointMatrix3x3<T>& matrix) noexcept;

} // namespace Detail
} // namespace Pomdog
