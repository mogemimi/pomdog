// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Quaternion represents a rotation around an axis in 3D space.
class POMDOG_EXPORT Quaternion final {
public:
    float X;
    float Y;
    float Z;
    float W;

public:
    /// Creates a new Quaternion object.
    Quaternion() noexcept;

    /// Construct from floating-point values.
    Quaternion(float x, float y, float z, float w) noexcept;

    // Assignment operators:
    Quaternion& operator+=(const Quaternion&) noexcept;
    Quaternion& operator-=(const Quaternion&) noexcept;
    Quaternion& operator*=(const Quaternion&) noexcept;
    Quaternion& operator*=(float factor) noexcept;
    Quaternion& operator/=(float factor) noexcept;

    // Unary operators:
    Quaternion operator+() const noexcept;
    Quaternion operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Quaternion operator+(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator-(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(float factor) const noexcept;
    [[nodiscard]] Quaternion operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Quaternion&) const noexcept;
    [[nodiscard]] bool operator!=(const Quaternion&) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    ///@param axis The normalized axis of the rotation.
    [[nodiscard]] static Quaternion
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    CreateFromRotationMatrix(const Matrix4x4& rotation);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    CreateFromRotationMatrix(const Matrix3x3& rotation);

    /// Creates a quaternion from the specified yaw, pitch, and roll.
    ///
    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    [[nodiscard]] static Quaternion
    CreateFromYawPitchRoll(const Radian<float>& yaw, const Radian<float>& pitch, const Radian<float>& roll);

    /// Returns a quaternion constructed from euler angles in radians.
    [[nodiscard]] static Quaternion
    Euler(const Vector3& rotation);

    /// Returns an euler angles of the given quaternion in radians.
    ///
    /// @return Euler angles in pitch-yaw-roll order, in radians.
    [[nodiscard]] static Vector3
    ToEulerAngles(const Quaternion& quaternion) noexcept;

    [[nodiscard]] static Quaternion
    Identity() noexcept;
};

/// Multiplies a quaternion by a scalar factor.
[[nodiscard]] Quaternion POMDOG_EXPORT
operator*(float factor, const Quaternion& quaternion) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a quaternion.
[[nodiscard]] float POMDOG_EXPORT
Length(const Quaternion& quaternion) noexcept;

/// Calculates and returns the squared length of a quaternion.
[[nodiscard]] float POMDOG_EXPORT
LengthSquared(const Quaternion& quaternion) noexcept;

/// Calculates and returns the dot product of two quaternions.
[[nodiscard]] float POMDOG_EXPORT
Dot(const Quaternion& a, const Quaternion& b) noexcept;

/// Returns a unit quaternion in the same rotation from the specified quaternion.
[[nodiscard]] Quaternion POMDOG_EXPORT
Normalize(const Quaternion& quaternion) noexcept;

/// Performs a linear interpolation between two quaternions.
[[nodiscard]] Quaternion POMDOG_EXPORT
Lerp(const Quaternion& source1, const Quaternion& source2, float amount);

/// Performs a spherical linear interpolation between two quaternions.
[[nodiscard]] Quaternion POMDOG_EXPORT
Slerp(const Quaternion& begin, const Quaternion& end, float amount);

/// Calculates and returns the inverse of a quaternion.
[[nodiscard]] Quaternion POMDOG_EXPORT
Invert(const Quaternion& source);

/// Rotates a vector by a quaternion.
[[nodiscard]] Vector3 POMDOG_EXPORT
Rotate(const Quaternion& quaternion, const Vector3& vector);

} // namespace pomdog::math
