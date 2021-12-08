// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename T>
class POMDOG_EXPORT FloatingPointQuaternion final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    using value_type = T;

    T X, Y, Z, W;

public:
    /// Creates a new FloatingPointQuaternion<T> object.
    FloatingPointQuaternion() noexcept = default;

    /// Construct from floating-point values.
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
    Lerp(const FloatingPointQuaternion& source1, const FloatingPointQuaternion& source2, T amount, FloatingPointQuaternion& result);

    [[nodiscard]] static FloatingPointQuaternion
    Lerp(const FloatingPointQuaternion& source1, const FloatingPointQuaternion& source2, T amount);

    static void
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount, FloatingPointQuaternion& result);

    [[nodiscard]] static FloatingPointQuaternion
    Slerp(const FloatingPointQuaternion& begin, const FloatingPointQuaternion& end, T amount);

    static void
    Inverse(const FloatingPointQuaternion& source, FloatingPointQuaternion& result);

    [[nodiscard]] static FloatingPointQuaternion
    Inverse(const FloatingPointQuaternion& source);

    /// Rotates a vector by a quaternion.
    [[nodiscard]] static FloatingPointVector3<T>
    Rotate(const FloatingPointQuaternion& quaternion, const FloatingPointVector3<T>& vector);

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

    /// Returns a quaternion constructed from euler angles in radians.
    [[nodiscard]] static FloatingPointQuaternion
    Euler(const FloatingPointVector3<T>& rotation);

    /// Returns an euler angles of the given quaternion in radians.
    ///
    /// @return Euler angles in pitch-yaw-roll order, in radians.
    [[nodiscard]] static FloatingPointVector3<T>
    ToEulerAngles(const FloatingPointQuaternion& quaternion) noexcept;

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

} // namespace pomdog::detail
