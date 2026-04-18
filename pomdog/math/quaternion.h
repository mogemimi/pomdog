// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector3;
class Matrix4x4;
class Matrix3x3;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Quaternion represents a rotation around an axis in 3D space.
class POMDOG_EXPORT Quaternion final {
public:
    f32 x;
    f32 y;
    f32 z;
    f32 w;

public:
    /// Creates a new Quaternion object.
    Quaternion() noexcept;

    /// Construct from floating-point values.
    Quaternion(f32 x, f32 y, f32 z, f32 w) noexcept;

    // Assignment operators:
    Quaternion& operator+=(const Quaternion&) noexcept;
    Quaternion& operator-=(const Quaternion&) noexcept;
    Quaternion& operator*=(const Quaternion&) noexcept;
    Quaternion& operator*=(f32 factor) noexcept;
    Quaternion& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Quaternion operator+() const noexcept;
    [[nodiscard]] Quaternion operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Quaternion operator+(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator-(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(f32 factor) const noexcept;
    [[nodiscard]] Quaternion operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Quaternion&) const noexcept;
    [[nodiscard]] bool operator!=(const Quaternion&) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    ///@param axis The normalized axis of the rotation.
    [[nodiscard]] static Quaternion
    createFromAxisAngle(const Vector3& axis, const Radian<f32>& angle);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    createFromRotationMatrix(const Matrix4x4& rotation);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    createFromRotationMatrix(const Matrix3x3& rotation);

    /// Creates a quaternion from the specified yaw, pitch, and roll.
    ///
    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    [[nodiscard]] static Quaternion
    createFromYawPitchRoll(const Radian<f32>& yaw, const Radian<f32>& pitch, const Radian<f32>& roll);

    /// Returns a quaternion constructed from euler angles in radians.
    [[nodiscard]] static Quaternion
    euler(const Vector3& rotation);

    /// Returns an euler angles of the given quaternion in radians.
    ///
    /// @return Euler angles in pitch-yaw-roll order, in radians.
    [[nodiscard]] static Vector3
    toEulerAngles(const Quaternion& quaternion) noexcept;

    [[nodiscard]] static Quaternion
    createIdentity() noexcept;
};

/// Multiplies a quaternion by a scalar factor.
[[nodiscard]] Quaternion POMDOG_EXPORT
operator*(f32 factor, const Quaternion& quaternion) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a quaternion.
[[nodiscard]] f32 POMDOG_EXPORT
length(const Quaternion& quaternion) noexcept;

/// Calculates and returns the squared length of a quaternion.
[[nodiscard]] f32 POMDOG_EXPORT
lengthSquared(const Quaternion& quaternion) noexcept;

/// Calculates and returns the dot product of two quaternions.
[[nodiscard]] f32 POMDOG_EXPORT
dot(const Quaternion& a, const Quaternion& b) noexcept;

/// Returns a unit quaternion in the same rotation from the specified quaternion.
[[nodiscard]] Quaternion POMDOG_EXPORT
normalize(const Quaternion& quaternion) noexcept;

/// Performs a linear interpolation between two quaternions.
[[nodiscard]] Quaternion POMDOG_EXPORT
lerp(const Quaternion& source1, const Quaternion& source2, f32 amount);

/// Performs a spherical linear interpolation between two quaternions.
[[nodiscard]] Quaternion POMDOG_EXPORT
slerp(const Quaternion& begin, const Quaternion& end, f32 amount);

/// Calculates and returns the inverse of a quaternion.
[[nodiscard]] Quaternion POMDOG_EXPORT
invert(const Quaternion& source);

/// Rotates a vector by a quaternion.
[[nodiscard]] Vector3 POMDOG_EXPORT
rotate(const Quaternion& quaternion, const Vector3& vector);

/// Returns a quaternion with each component set to its absolute value.
[[nodiscard]] Quaternion POMDOG_EXPORT
abs(const Quaternion& source) noexcept;

/// Clamps each component of the quaternion between 0 and 1.
[[nodiscard]] Quaternion POMDOG_EXPORT
saturate(const Quaternion& source) noexcept;

/// Restricts each component of a quaternion between a minimum and a maximum value.
[[nodiscard]] Quaternion POMDOG_EXPORT
clamp(const Quaternion& source, const Quaternion& min, const Quaternion& max) noexcept;

/// Performs Hermite interpolation between two quaternions component-wise.
[[nodiscard]] Quaternion POMDOG_EXPORT
smoothstep(const Quaternion& source1, const Quaternion& source2, f32 amount);

} // namespace pomdog::math
