// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog {
namespace Detail {

///@brief vector in two-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector2 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y;

public:
    // Constructors:
    FloatingPointVector2() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector2(T x, T y) noexcept;

    // Assignment operators:
    FloatingPointVector2& operator+=(const FloatingPointVector2&) noexcept;
    FloatingPointVector2& operator-=(const FloatingPointVector2&) noexcept;
    FloatingPointVector2& operator*=(T) noexcept;
    FloatingPointVector2& operator/=(T);

    // Unary operators:
    FloatingPointVector2 operator+() const noexcept;
    FloatingPointVector2 operator-() const noexcept;

    // Binary operators:
    FloatingPointVector2 operator+(const FloatingPointVector2&) const noexcept;
    FloatingPointVector2 operator-(const FloatingPointVector2&) const noexcept;
    FloatingPointVector2 operator*(const FloatingPointVector2&) const noexcept;
    FloatingPointVector2 operator/(const FloatingPointVector2&) const;

    FloatingPointVector2 operator*(T scaleFactor) const noexcept;
    FloatingPointVector2 operator/(T scaleFactor) const;

    bool operator==(const FloatingPointVector2& other) const noexcept;
    bool operator!=(const FloatingPointVector2& other) const noexcept;

    T Length() const noexcept;

    T LengthSquared() const noexcept;

    static T Distance(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static T DistanceSquared(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static T Dot(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static T Cross(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static FloatingPointVector2 Min(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static FloatingPointVector2 Max(const FloatingPointVector2& a, const FloatingPointVector2& b) noexcept;

    static FloatingPointVector2 Clamp(
        const FloatingPointVector2& source,
        const FloatingPointVector2& min,
        const FloatingPointVector2& max) noexcept;

    void Normalize() noexcept;

    static FloatingPointVector2 Normalize(const FloatingPointVector2& source) noexcept;

    static void Normalize(const FloatingPointVector2& source, FloatingPointVector2& result) noexcept;

    static FloatingPointVector2 Transform(
        const FloatingPointVector2& position,
        const FloatingPointMatrix3x2<T>& matrix) noexcept;

    static FloatingPointVector2 Transform(
        const FloatingPointVector2& position,
        const FloatingPointMatrix4x4<T>& matrix) noexcept;

    static FloatingPointVector2 Transform(
        const FloatingPointVector2& position,
        const FloatingPointQuaternion<T>& rotation) noexcept;

    static FloatingPointVector2 TransformNormal(
        const FloatingPointVector2& normal,
        const FloatingPointMatrix4x4<T>& matrix) noexcept;

    static FloatingPointVector2 Lerp(
        const FloatingPointVector2& source1,
        const FloatingPointVector2& source2,
        T amount);

    static FloatingPointVector2 SmoothStep(
        const FloatingPointVector2& source1,
        const FloatingPointVector2& source2,
        T amount);

    ///@brief Returns pointer to the first element.
    const T* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    ///@note (0, 0)
    static FloatingPointVector2 const Zero;
};

template <typename T>
FloatingPointVector2<T> const FloatingPointVector2<T>::Zero = {0, 0};

template <typename T>
FloatingPointVector2<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointVector2<T>& vector) noexcept;

} // namespace Detail
} // namespace Pomdog
