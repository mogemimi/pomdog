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

/// Vector2 is a vector in 2-dimensional space.
class POMDOG_EXPORT Vector2 final {
public:
    float X;
    float Y;

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
    Vector2 operator+() const noexcept;
    Vector2 operator-() const noexcept;

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
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    /// Returns the Vector2 (0, 0).
    [[nodiscard]] static Vector2 Zero() noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] Vector2 POMDOG_EXPORT
operator*(float factor, const Vector2& vector) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the length of a vector.
[[nodiscard]] float POMDOG_EXPORT
Length(const Vector2& v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] float POMDOG_EXPORT
LengthSquared(const Vector2& v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
Distance(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] float POMDOG_EXPORT
DistanceSquared(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] float POMDOG_EXPORT
Dot(const Vector2& a, const Vector2& b) noexcept;

/// Calculates and returns the cross product of two vectors.
[[nodiscard]] float POMDOG_EXPORT
Cross(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
Min(const Vector2& a, const Vector2& b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
Max(const Vector2& a, const Vector2& b) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] Vector2 POMDOG_EXPORT
Clamp(const Vector2& source, const Vector2& min, const Vector2& max) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] Vector2 POMDOG_EXPORT
Normalize(const Vector2& source) noexcept;

/// Rotates a vector by a given angle in radians.
[[nodiscard]] Vector2 POMDOG_EXPORT
Rotate(const Vector2& vector, const Radian<float>& radian) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
Transform(const Vector2& position, const Matrix3x2& matrix) noexcept;

/// Returns a transformed vector by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
Transform(const Vector2& position, const Matrix4x4& matrix) noexcept;

/// Returns a transformed vector by the specified quaternion.
[[nodiscard]] Vector2 POMDOG_EXPORT
Transform(const Vector2& position, const Quaternion& rotation) noexcept;

/// Returns a transformed vector normal by the specified matrix.
[[nodiscard]] Vector2 POMDOG_EXPORT
TransformNormal(const Vector2& normal, const Matrix4x4& matrix) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
Lerp(const Vector2& source1, const Vector2& source2, float amount);

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] Vector2 POMDOG_EXPORT
SmoothStep(const Vector2& source1, const Vector2& source2, float amount);

} // namespace pomdog::math
