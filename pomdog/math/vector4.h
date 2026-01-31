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
[[nodiscard]] Vector4 POMDOG_EXPORT
operator*(f32 factor, const Vector4& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] f32 POMDOG_EXPORT
length(const Vector4& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] f32 POMDOG_EXPORT
lengthSquared(const Vector4& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] f32 POMDOG_EXPORT
distance(const Vector4& a, const Vector4& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] f32 POMDOG_EXPORT
distanceSquared(const Vector4& a, const Vector4& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] f32 POMDOG_EXPORT
dot(const Vector4& a, const Vector4& b) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] Vector4 POMDOG_EXPORT
normalize(const Vector4& source) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector4 POMDOG_EXPORT
transform(const Vector4& vector, const Matrix4x4& matrix) noexcept;

} // namespace pomdog::math
