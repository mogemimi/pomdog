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
class POMDOG_EXPORT FloatingPointMatrix3x2 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T m[3][2];

private:
    static constexpr std::size_t RowSize = 3;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    FloatingPointMatrix3x2() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointMatrix3x2(
        T m00, T m01,
        T m10, T m11,
        T m20, T m21) noexcept;

    // Assignment operators:
    FloatingPointMatrix3x2& operator*=(const FloatingPointMatrix3x2& other) noexcept;
    FloatingPointMatrix3x2& operator+=(const FloatingPointMatrix3x2& other) noexcept;
    FloatingPointMatrix3x2& operator-=(const FloatingPointMatrix3x2& other) noexcept;
    FloatingPointMatrix3x2& operator*=(T scaleFactor) noexcept;
    FloatingPointMatrix3x2& operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix3x2 operator+() const noexcept;
    FloatingPointMatrix3x2 operator-() const noexcept;

    // Binary operators:
    FloatingPointMatrix3x2 operator+(const FloatingPointMatrix3x2& other) const noexcept;
    FloatingPointMatrix3x2 operator-(const FloatingPointMatrix3x2& other) const noexcept;
    FloatingPointMatrix3x2 operator*(const FloatingPointMatrix3x2& other) const noexcept;
    FloatingPointMatrix3x2 operator*(T scaleFactor) const noexcept;
    FloatingPointMatrix3x2 operator/(T scaleFactor) const;

    bool operator==(const FloatingPointMatrix3x2& other) const noexcept;
    bool operator!=(const FloatingPointMatrix3x2& other) const noexcept;

    // Function-call operators:
    const T& operator()(std::size_t row, std::size_t column) const;
    T& operator()(std::size_t row, std::size_t column);

    T Determinant() const noexcept;

    static FloatingPointMatrix3x2
    Multiply(const FloatingPointMatrix3x2& matrix1, const FloatingPointMatrix3x2& matrix2) noexcept;

    static FloatingPointMatrix3x2
    Multiply(const FloatingPointMatrix3x2& matrix1, T scaleFactor) noexcept;

    static void
    Invert(const FloatingPointMatrix3x2& matrix, FloatingPointMatrix3x2& result);

    static FloatingPointMatrix3x2
    Invert(const FloatingPointMatrix3x2& matrix);

    static void
    Lerp(const FloatingPointMatrix3x2& source1, const FloatingPointMatrix3x2& source2,
        T amount, FloatingPointMatrix3x2& result) noexcept;

    static FloatingPointMatrix3x2
    Lerp(const FloatingPointMatrix3x2& source1, const FloatingPointMatrix3x2& source2,
        T amount) noexcept;

    static void
    CreateTranslation(const FloatingPointVector2<T>& position, FloatingPointMatrix3x2& result) noexcept;

    static FloatingPointMatrix3x2
    CreateTranslation(const FloatingPointVector2<T>& position) noexcept;

    static void
    CreateScale(T scale, FloatingPointMatrix3x2& result) noexcept;

    static FloatingPointMatrix3x2
    CreateScale(T scale) noexcept;

    static void
    CreateScale(const FloatingPointVector2<T>& scale, FloatingPointMatrix3x2& result) noexcept;

    static FloatingPointMatrix3x2
    CreateScale(const FloatingPointVector2<T>& scale) noexcept;

    static void
    CreateRotation(const Radian<T>& angle, FloatingPointMatrix3x2& result);

    static FloatingPointMatrix3x2
    CreateRotation(const Radian<T>& angle);

    static void
    CreateSkew(const FloatingPointVector2<T>& skew, FloatingPointMatrix3x2& result);

    static FloatingPointMatrix3x2
    CreateSkew(const FloatingPointVector2<T>& skew);

    ///@brief Returns pointer to the first element.
    const T* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointMatrix3x2 const Identity;
};

template <typename T>
FloatingPointMatrix3x2<T> const FloatingPointMatrix3x2<T>::Identity = {
    1, 0,
    0, 1,
    0, 0,
};

template <typename T>
FloatingPointMatrix3x2<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointMatrix3x2<T>& matrix) noexcept;

} // namespace Detail
} // namespace Pomdog
