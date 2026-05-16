// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector3;
class Matrix4x4;
} // namespace pomdog

namespace pomdog {

/// Vector4 is a vector in 4-dimensional space.
class POMDOG_EXPORT Vector4 final {
public:
    f32 x;
    f32 y;
    f32 z;
    f32 w;

public:
    /// Creates a new Vector4 object.
    Vector4() noexcept;

    /// Constructs from floating-point values.
    Vector4(f32 x, f32 y, f32 z, f32 w) noexcept;
    Vector4(const Vector3& xyz, f32 w) noexcept;

    // Assignment operators:
    Vector4& operator+=(const Vector4& other) noexcept;
    Vector4& operator-=(const Vector4& other) noexcept;
    Vector4& operator*=(f32 factor) noexcept;
    Vector4& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Vector4 operator+() const noexcept;
    [[nodiscard]] Vector4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector4 operator+(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator-(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator*(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator/(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator*(f32 factor) const noexcept;
    [[nodiscard]] Vector4 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Vector4& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector4& other) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] POMDOG_EXPORT Vector4
operator*(f32 factor, const Vector4& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
length(const Vector4& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
lengthSquared(const Vector4& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distance(const Vector4& a, const Vector4& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distanceSquared(const Vector4& a, const Vector4& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] POMDOG_EXPORT f32
dot(const Vector4& a, const Vector4& b) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] POMDOG_EXPORT Vector4
normalize(const Vector4& source) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] POMDOG_EXPORT Vector4
transform(const Vector4& vector, const Matrix4x4& matrix) noexcept;

/// Returns a vector with each component set to its absolute value.
[[nodiscard]] POMDOG_EXPORT Vector4
abs(const Vector4& source) noexcept;

/// Returns a vector with each component rounded down to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector4
floor(const Vector4& source) noexcept;

/// Returns a vector with each component rounded up to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector4
ceil(const Vector4& source) noexcept;

/// Returns a vector with each component rounded to the nearest integer.
[[nodiscard]] POMDOG_EXPORT Vector4
round(const Vector4& source) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] POMDOG_EXPORT Vector4
min(const Vector4& a, const Vector4& b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] POMDOG_EXPORT Vector4
max(const Vector4& a, const Vector4& b) noexcept;

/// Clamps each component of the vector between 0 and 1.
[[nodiscard]] POMDOG_EXPORT Vector4
saturate(const Vector4& source) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] POMDOG_EXPORT Vector4
clamp(const Vector4& source, const Vector4& min, const Vector4& max) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT Vector4
lerp(const Vector4& source1, const Vector4& source2, f32 amount);

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT Vector4
smoothstep(const Vector4& source1, const Vector4& source2, f32 amount);

} // namespace pomdog::math
