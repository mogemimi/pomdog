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
    FloatingPointQuaternion operator+(const FloatingPointQuaternion&) const noexcept;
    FloatingPointQuaternion operator-(const FloatingPointQuaternion&) const noexcept;
    FloatingPointQuaternion operator*(const FloatingPointQuaternion&) const noexcept;
    FloatingPointQuaternion operator*(T scaleFactor) const noexcept;
    FloatingPointQuaternion operator/(T scaleFactor) const;

    bool operator==(const FloatingPointQuaternion&) const noexcept;
    bool operator!=(const FloatingPointQuaternion&) const noexcept;

    T Length() const noexcept;

    T LengthSquared() const noexcept;

    static T Dot(const FloatingPointQuaternion& a, const FloatingPointQuaternion& b) noexcept;

    void Normalize() noexcept;

    static FloatingPointQuaternion Normalize(const FloatingPointQuaternion& quaternion) noexcept;

    static void Normalize(const FloatingPointQuaternion& quaternion, FloatingPointQuaternion& result) noexcept;

    static void
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount, FloatingPointQuaternion& result);

    static FloatingPointQuaternion
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount);

    static void
    Inverse(const FloatingPointQuaternion& source, FloatingPointQuaternion& result);

    static FloatingPointQuaternion
    Inverse(const FloatingPointQuaternion& source);

    ///@param axis The normalized axis of the rotation.
    static void
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle, FloatingPointQuaternion& result);

    ///@param axis The normalized axis of the rotation.
    static FloatingPointQuaternion
    CreateFromAxisAngle(const FloatingPointVector3<T>& axis, const Radian<T>& angle);

    ///@brief Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation, FloatingPointQuaternion& result);

    ///@brief Convert rotation matrix to quaternion.
    static FloatingPointQuaternion
    CreateFromRotationMatrix(const FloatingPointMatrix4x4<T>& rotation);

    ///@brief Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation, FloatingPointQuaternion& result);

    ///@brief Convert rotation matrix to quaternion.
    static FloatingPointQuaternion
    CreateFromRotationMatrix(const FloatingPointMatrix3x3<T>& rotation);

    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    static void
    CreateFromYawPitchRoll(const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll, FloatingPointQuaternion& result);

    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    static FloatingPointQuaternion
    CreateFromYawPitchRoll(const Radian<T>& yaw, const Radian<T>& pitch, const Radian<T>& roll);

    ///@brief Returns pointer to the first element.
    const T* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointQuaternion const Identity;
};

template <typename T>
FloatingPointQuaternion<T> const FloatingPointQuaternion<T>::Identity = {0, 0, 0, 1};

template <typename T>
FloatingPointQuaternion<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointQuaternion<T>& quaternion) noexcept;

} // namespace Detail
} // namespace Pomdog
