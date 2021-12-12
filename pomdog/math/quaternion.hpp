// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

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
    Quaternion& operator*=(float scaleFactor) noexcept;
    Quaternion& operator/=(float scaleFactor);

    // Unary operators:
    Quaternion operator+() const noexcept;
    Quaternion operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Quaternion operator+(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator-(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(const Quaternion&) const noexcept;
    [[nodiscard]] Quaternion operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Quaternion operator/(float scaleFactor) const;

    [[nodiscard]] bool operator==(const Quaternion&) const noexcept;
    [[nodiscard]] bool operator!=(const Quaternion&) const noexcept;

    [[nodiscard]] float Length() const noexcept;

    [[nodiscard]] float LengthSquared() const noexcept;

    [[nodiscard]] static float
    Dot(const Quaternion& a, const Quaternion& b) noexcept;

    void Normalize() noexcept;

    [[nodiscard]] static Quaternion
    Normalize(const Quaternion& quaternion) noexcept;

    static void
    Normalize(const Quaternion& quaternion, Quaternion& result) noexcept;

    static void
    Lerp(const Quaternion& source1, const Quaternion& source2, float amount, Quaternion& result);

    [[nodiscard]] static Quaternion
    Lerp(const Quaternion& source1, const Quaternion& source2, float amount);

    static void
    Slerp(const Quaternion& begin, const Quaternion& end, float amount, Quaternion& result);

    [[nodiscard]] static Quaternion
    Slerp(const Quaternion& begin, const Quaternion& end, float amount);

    static void
    Inverse(const Quaternion& source, Quaternion& result);

    [[nodiscard]] static Quaternion
    Inverse(const Quaternion& source);

    /// Rotates a vector by a quaternion.
    [[nodiscard]] static Vector3
    Rotate(const Quaternion& quaternion, const Vector3& vector);

    ///@param axis The normalized axis of the rotation.
    static void
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle, Quaternion& result);

    ///@param axis The normalized axis of the rotation.
    [[nodiscard]] static Quaternion
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const Matrix4x4& rotation, Quaternion& result);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    CreateFromRotationMatrix(const Matrix4x4& rotation);

    /// Convert rotation matrix to quaternion.
    static void
    CreateFromRotationMatrix(const Matrix3x3& rotation, Quaternion& result);

    /// Convert rotation matrix to quaternion.
    [[nodiscard]] static Quaternion
    CreateFromRotationMatrix(const Matrix3x3& rotation);

    /// Creates a quaternion from the specified yaw, pitch, and roll.
    ///
    ///@param yaw The rotation around the y-axis in radians.
    ///@param pitch The rotation around the x-axis in radians.
    ///@param roll The rotation around the z-axis in radians.
    static void
    CreateFromYawPitchRoll(const Radian<float>& yaw, const Radian<float>& pitch, const Radian<float>& roll, Quaternion& result);

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

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    static Quaternion const Identity;
};

[[nodiscard]] Quaternion POMDOG_EXPORT
operator*(float scaleFactor, const Quaternion& quaternion) noexcept;

} // namespace pomdog
