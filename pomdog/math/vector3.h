// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Matrix4x4;
class Quaternion;
class Vector2;
} // namespace pomdog

namespace pomdog {

/// Vector3 is a vector in 3-dimensional space.
class POMDOG_EXPORT Vector3 final {
public:
    float x;
    float y;
    float z;

public:
    /// Creates a new Vector3 object.
    Vector3() noexcept;

    /// Constructs from floating-point values.
    Vector3(float x, float y, float z) noexcept;

    Vector3(const Vector2& vec, float z) noexcept;

    // Assignment operators:
    Vector3& operator+=(const Vector3&) noexcept;
    Vector3& operator-=(const Vector3&) noexcept;
    Vector3& operator*=(float) noexcept;
    Vector3& operator/=(float) noexcept;

    // Unary operators:
    [[nodiscard]] Vector3 operator+() const noexcept;
    [[nodiscard]] Vector3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector3 operator+(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator-(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator*(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator/(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator*(float factor) const noexcept;
    [[nodiscard]] Vector3 operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Vector3& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector3& other) const noexcept;

    [[nodiscard]] static Vector3
    toEulerAngles(const Quaternion& quaternion);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* data() noexcept;

    /// Returns the Vector3 (0, 0, 0).
    [[nodiscard]] static Vector3
    createZero() noexcept;

    /// Returns the Vector3 (1, 0, 0).
    [[nodiscard]] static Vector3
    createUnitX() noexcept;

    /// Returns the Vector3 (0, 1, 0).
    [[nodiscard]] static Vector3
    createUnitY() noexcept;

    /// Returns the Vector3 (0, 0, 1).
    [[nodiscard]] static Vector3
    createUnitZ() noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] Vector3 POMDOG_EXPORT
operator*(float factor, const Vector3& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] float POMDOG_EXPORT
length(const Vector3& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] float POMDOG_EXPORT
lengthSquared(const Vector3& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
distance(const Vector3& a, const Vector3& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
distanceSquared(const Vector3& a, const Vector3& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] float POMDOG_EXPORT
dot(const Vector3& a, const Vector3& b) noexcept;

/// Calculates and returns the cross product of two vectors.
[[nodiscard]] Vector3 POMDOG_EXPORT
cross(const Vector3& a, const Vector3& b) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] Vector3 POMDOG_EXPORT
min(const Vector3& a, const Vector3& b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] Vector3 POMDOG_EXPORT
max(const Vector3& a, const Vector3& b) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] Vector3 POMDOG_EXPORT
clamp(const Vector3& source, const Vector3& min, const Vector3& max) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] Vector3 POMDOG_EXPORT
lerp(const Vector3& source1, const Vector3& source2, float amount);

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] Vector3 POMDOG_EXPORT
smoothstep(const Vector3& source1, const Vector3& source2, float amount);

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] Vector3 POMDOG_EXPORT
normalize(const Vector3& source) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector3 POMDOG_EXPORT
transform(const Vector3& position, const Matrix4x4& matrix) noexcept;

/// Returns a transformed vector normal by the specified matrix.
[[nodiscard]] Vector3 POMDOG_EXPORT
transformNormal(const Vector3& normal, const Matrix4x4& matrix) noexcept;

} // namespace pomdog::math
