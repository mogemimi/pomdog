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
    f32 x;
    f32 y;

public:
    /// Creates a new Vector2 object.
    Vector2() noexcept;

    /// Constructs from floating-point values.
    Vector2(f32 x, f32 y) noexcept;

    // Assignment operators:
    Vector2& operator+=(const Vector2&) noexcept;
    Vector2& operator-=(const Vector2&) noexcept;
    Vector2& operator*=(f32) noexcept;
    Vector2& operator/=(f32) noexcept;

    // Unary operators:
    [[nodiscard]] Vector2 operator+() const noexcept;
    [[nodiscard]] Vector2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector2 operator+(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator-(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator*(const Vector2&) const noexcept;
    [[nodiscard]] Vector2 operator/(const Vector2&) const noexcept;

    [[nodiscard]] Vector2 operator*(f32 factor) const noexcept;
    [[nodiscard]] Vector2 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Vector2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector2& other) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    /// Returns the Vector2 (0, 0).
    [[nodiscard]] static Vector2
    createZero() noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] POMDOG_EXPORT Vector2
operator*(f32 factor, const Vector2& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
length(const Vector2& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
lengthSquared(const Vector2& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distance(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distanceSquared(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] POMDOG_EXPORT f32
dot(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the cross product of two vectors.
[[nodiscard]] POMDOG_EXPORT f32
cross(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] POMDOG_EXPORT Vector2
min(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] POMDOG_EXPORT Vector2
max(const Vector2& a, const Vector2& b) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] POMDOG_EXPORT Vector2
clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] POMDOG_EXPORT Vector2
normalize(const Vector2& source) noexcept;

/// Rotates a vector by a given angle in radians.
[[nodiscard]] POMDOG_EXPORT Vector2
rotate(const Vector2& vector, const Radian<f32>& radian) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] POMDOG_EXPORT Vector2
transform(const Vector2& position, const Matrix3x2& matrix) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] POMDOG_EXPORT Vector2
transform(const Vector2& position, const Matrix4x4& matrix) noexcept;

/// Returns a transformed vector by the specified quaternion.
[[nodiscard]] POMDOG_EXPORT Vector2
transform(const Vector2& position, const Quaternion& rotation) noexcept;

/// Returns a transformed vector normal by the specified matrix.
[[nodiscard]] POMDOG_EXPORT Vector2
transformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT Vector2
lerp(const Vector2& source1, const Vector2& source2, f32 amount);

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT Vector2
smoothstep(const Vector2& source1, const Vector2& source2, f32 amount);

/// Returns a vector with each component set to its absolute value.
[[nodiscard]] POMDOG_EXPORT Vector2
abs(const Vector2& source) noexcept;

/// Returns a vector with each component rounded down to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector2
floor(const Vector2& source) noexcept;

/// Returns a vector with each component rounded up to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector2
ceil(const Vector2& source) noexcept;

/// Returns a vector with each component rounded to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector2
round(const Vector2& source) noexcept;

/// Clamps each component of the vector between 0 and 1.
[[nodiscard]] POMDOG_EXPORT Vector2
saturate(const Vector2& source) noexcept;

/// Normalizes an angle in radians to the range [0, 2*pi).
[[nodiscard]] POMDOG_EXPORT f32
toPositiveAngle(const Vector2& source) noexcept;

/// Normalizes an angle in radians to the range (-pi, pi].
[[nodiscard]] POMDOG_EXPORT f32
toSignedAngle(const Vector2& source) noexcept;

/// Returns true if two vectors are approximately equal within the default tolerance.
[[nodiscard]] POMDOG_EXPORT bool
approxEqual(const Vector2& a, const Vector2& b) noexcept;

/// Returns true if two vectors are approximately equal within the specified tolerance.
[[nodiscard]] POMDOG_EXPORT bool
approxEqual(const Vector2& a, const Vector2& b, f32 tolerance) noexcept;

} // namespace pomdog::math
