// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog::Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointQuaternion final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T X, Y, Z, W;

public:
    // Constructors:
    FloatingPointQuaternion() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointQuaternion(T x, T y, T z, T w) noexcept;

    // Assignment operators:
    FloatingPointQuaternion& operator+=(const FloatingPointQuaternion&) noexcept;
    FloatingPointQuaternion& operator-=(const FloatingPointQuaternion&) noexcept;
    FloatingPointQuaternion& operator*=(const FloatingPointQuaternion&) noexcept;
    FloatingPointQuaternion& operator*=(T scaleFactor) noexcept;
    FloatingPointQuaternion& operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointQuaternion operator+() const noexcept;
    FloatingPointQuaternion operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] FloatingPointQuaternion operator+(const FloatingPointQuaternion&) const noexcept;
    [[nodiscard]] FloatingPointQuaternion operator-(const FloatingPointQuaternion&) const noexcept;
    [[nodiscard]] FloatingPointQuaternion operator*(const FloatingPointQuaternion&) const noexcept;
    [[nodiscard]] FloatingPointQuaternion operator*(T scaleFactor) const noexcept;
    [[nodiscard]] FloatingPointQuaternion operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const FloatingPointQuaternion&) const noexcept;
    [[nodiscard]] bool operator!=(const FloatingPointQuaternion&) const noexcept;

    [[nodiscard]] T Length() const noexcept;

    [[nodiscard]] T LengthSquared() const noexcept;

    [[nodiscard]] static T
    Dot(const FloatingPointQuaternion& a, const FloatingPointQuaternion& b) noexcept;

    void Normalize() noexcept;

    [[nodiscard]] static FloatingPointQuaternion
    Normalize(const FloatingPointQuaternion& quaternion) noexcept;

    static void
    Normalize(const FloatingPointQuaternion& quaternion, FloatingPointQuaternion& result) noexcept;

    static void
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount, FloatingPointQuaternion& result);

    [[nodiscard]] static FloatingPointQuaternion
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount);

    static void
    Inverse(const FloatingPointQuaternion& source, FloatingPointQuaternion& result);

    [[nodiscard]] static FloatingPointQuaternion
    Inverse(const FloatingPointQuaternion& source);

    ///@param axis The normalized axis of the rotation.
    static void
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle, FloatingPointQuaternion& result);

    ///@param axis The normalized axis of the rotation.
    [[nodiscard]] static FloatingPointQuaternion
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle);

    /// Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation, FloatingPointQuaternion& result);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static FloatingPointQuaternion
    CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation);

    /// Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation, FloatingPointQuaternion& result);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static FloatingPointQuaternion
    CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation);

    /// Creates a quaternion from the specified yaw, pitch, and roll.
    ///
    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    static void
    CreateFromYawPitchRoll(const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll, FloatingPointQuaternion& result);

    /// Creates a quaternion from the specified yaw, pitch, and roll.
    ///
    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    [[nodiscard]] static FloatingPointQuaternion
    CreateFromYawPitchRoll(const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll);

    /// Returns pointer to the first element.
    [[nodiscard]] const T* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] T* Data() noexcept;

    static FloatingPointQuaternion const Identity;
};

template <typename T>
FloatingPointQuaternion<T> const FloatingPointQuaternion<T>::Identity = {0, 0, 0, 1};

template <typename T>
FloatingPointQuaternion<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointQuaternion<T>& quaternion) noexcept;

} // namespace Pomdog::Detail
