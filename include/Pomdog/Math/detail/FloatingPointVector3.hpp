// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog::Detail {

///@brief vector in three-dimensional space.
template <typename T>
class POMDOG_EXPORT FloatingPointVector3 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y, Z;

public:
    // Constructors:
    FloatingPointVector3() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointVector3(T x, T y, T z) noexcept;

    FloatingPointVector3(const FloatingPointVector2<T>& vec, T z) noexcept;

    // Assignment operators:
    FloatingPointVector3& operator+=(const FloatingPointVector3&) noexcept;
    FloatingPointVector3& operator-=(const FloatingPointVector3&) noexcept;
    FloatingPointVector3& operator*=(T) noexcept;
    FloatingPointVector3& operator/=(T);

    // Unary operators:
    FloatingPointVector3 operator+() const noexcept;
    FloatingPointVector3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] FloatingPointVector3 operator+(const FloatingPointVector3&) const noexcept;
    [[nodiscard]] FloatingPointVector3 operator-(const FloatingPointVector3&) const noexcept;
    [[nodiscard]] FloatingPointVector3 operator*(const FloatingPointVector3&) const noexcept;
    [[nodiscard]] FloatingPointVector3 operator/(const FloatingPointVector3&) const;
    [[nodiscard]] FloatingPointVector3 operator*(T scaleFactor) const noexcept;
    [[nodiscard]] FloatingPointVector3 operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const FloatingPointVector3& other) const noexcept;
    [[nodiscard]] bool operator!=(const FloatingPointVector3& other) const noexcept;

    [[nodiscard]] T Length() const noexcept;

    [[nodiscard]] T LengthSquared() const noexcept;

    [[nodiscard]] static T
    Distance(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static T
    DistanceSquared(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static T
    Dot(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Cross(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Min(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Max(const FloatingPointVector3& a, const FloatingPointVector3& b) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Clamp(
        const FloatingPointVector3& source,
        const FloatingPointVector3& min,
        const FloatingPointVector3& max) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Lerp(
        const FloatingPointVector3& source1,
        const FloatingPointVector3& source2,
        T amount);

    [[nodiscard]] static FloatingPointVector3
    SmoothStep(
        const FloatingPointVector3& source1,
        const FloatingPointVector3& source2,
        T amount);

    void Normalize() noexcept;

    [[nodiscard]] static FloatingPointVector3
    Normalize(const FloatingPointVector3& source) noexcept;

    static void
    Normalize(const FloatingPointVector3&, FloatingPointVector3& result) noexcept;

    [[nodiscard]] static FloatingPointVector3
    Transform(
        const FloatingPointVector3& position,
        const FloatingPointMatrix4x4<T>& matrix) noexcept;

    [[nodiscard]] static FloatingPointVector3
    TransformNormal(
        const FloatingPointVector3& normal,
        const FloatingPointMatrix4x4<T>& matrix) noexcept;

    [[nodiscard]] static FloatingPointVector3
    ToEulerAngles(const FloatingPointQuaternion<T>& quaternion);

    /// Returns pointer to the first element.
    [[nodiscard]] const T* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] T* Data() noexcept;

    ///@note (0, 0, 0)
    static FloatingPointVector3 const Zero;

    ///@note (1, 0, 0)
    static FloatingPointVector3 const UnitX;

    ///@note (0, 1, 0)
    static FloatingPointVector3 const UnitY;

    ///@note (0, 0, 1)
    static FloatingPointVector3 const UnitZ;
};

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::Zero = {0, 0, 0};

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitX = {1, 0, 0};

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitY = {0, 1, 0};

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitZ = {0, 0, 1};

template <typename T>
FloatingPointVector3<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointVector3<T>& vector) noexcept;

} // namespace Pomdog::Detail
