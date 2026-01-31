// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Matrix3x2;
class Matrix4x4;
class Quaternion;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Vector2 is a vector in 2-dimensional space.
class POMDOG_EXPORT Vector2 final {
public:
    float x;
    float y;

public:
    /// Creates a new Vector2 object.
    Vector2() noexcept;

    /// Constructs from floating-point values.
    Vector2(float x, float y) noexcept;

    // Assignment operators:
    Vector2& operator+=(const Vector2&) noexcept;
    Vector2& operator-=(const Vector2&) noexcept;
    Vector2& operator*=(float) noexcept;
    Vector2& operator/=(float) noexcept;

    // Unary operators:
    [[nodiscard]] Vector2 operator+() const noexcept;
    [[nodiscard]] Vector2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector2 operator+(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator-(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator*(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator/(const Vector2&) const noexcept;

    [[nodiscard]] Vector2 operator*(float factor) const noexcept;
    [[nodiscard]] Vector2 operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Vector2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector2& other) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const float* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* data() noexcept;

    /// Returns the Vector2 (0, 0).
    [[nodiscard]] static Vector2
    createZero() noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] Vector2 POMDOG_EXPORT
operator*(float factor, const Vector2& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] float POMDOG_EXPORT
length(const Vector2& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] float POMDOG_EXPORT
lengthSquared(const Vector2& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
distance(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
distanceSquared(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] float POMDOG_EXPORT
dot(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the cross product of two vectors.
[[nodiscard]] float POMDOG_EXPORT
cross(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
min(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
max(const Vector2& a, const Vector2& b) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] Vector2 POMDOG_EXPORT
clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] Vector2 POMDOG_EXPORT
normalize(const Vector2& source) noexcept;

/// Rotates a vector by a given angle in radians.
[[nodiscard]] Vector2 POMDOG_EXPORT
rotate(const Vector2& vector, const Radian<f32>& radian) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
transform(const Vector2& position, const Matrix3x2& matrix) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
transform(const Vector2& position, const Matrix4x4& matrix) noexcept;

/// Returns a transformed vector by the specified quaternion.
[[nodiscard]] Vector2 POMDOG_EXPORT
transform(const Vector2& position, const Quaternion& rotation) noexcept;

/// Returns a transformed vector normal by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
transformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
lerp(const Vector2& source1, const Vector2& source2, float amount);

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
smoothstep(const Vector2& source1, const Vector2& source2, float amount);

} // namespace pomdog::math
