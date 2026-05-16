// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
template <typename T>
class Radian;
class Vector2;
} // namespace pomdog

namespace pomdog {

/// VectorXZ is a vector in 2-dimensional XZ space.
class POMDOG_EXPORT VectorXZ final {
public:
    f32 x;
    f32 z;

public:
    /// Creates a new VectorXZ object.
    VectorXZ() noexcept;

    /// Constructs from floating-point values.
    VectorXZ(f32 x, f32 z) noexcept;

    // Assignment operators:
    VectorXZ& operator+=(const VectorXZ&) noexcept;
    VectorXZ& operator-=(const VectorXZ&) noexcept;
    VectorXZ& operator*=(f32) noexcept;
    VectorXZ& operator/=(f32) noexcept;

    // Unary operators:
    [[nodiscard]] VectorXZ operator+() const noexcept;
    [[nodiscard]] VectorXZ operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] VectorXZ operator+(const VectorXZ&) const noexcept;
    [[nodiscard]] VectorXZ operator-(const VectorXZ&) const noexcept;
    [[nodiscard]] VectorXZ operator*(const VectorXZ&) const noexcept;
    [[nodiscard]] VectorXZ operator/(const VectorXZ&) const noexcept;

    [[nodiscard]] VectorXZ operator*(f32 scaleFactor) const noexcept;
    [[nodiscard]] VectorXZ operator/(f32 scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const VectorXZ& other) const noexcept;
    [[nodiscard]] bool operator!=(const VectorXZ& other) const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;
};

/// Multiplies a vector by a scalar factor.
[[nodiscard]] POMDOG_EXPORT VectorXZ
operator*(f32 scaleFactor, VectorXZ v) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Returns the component-wise absolute value of a vector.
[[nodiscard]] POMDOG_EXPORT VectorXZ
abs(VectorXZ v) noexcept;

/// Returns the component-wise floor of a vector.
[[nodiscard]] POMDOG_EXPORT VectorXZ
floor(VectorXZ v) noexcept;

/// Returns the component-wise round of a vector.
[[nodiscard]] POMDOG_EXPORT VectorXZ
round(VectorXZ v) noexcept;

/// Returns the component-wise ceiling of a vector.
[[nodiscard]] POMDOG_EXPORT VectorXZ
ceil(VectorXZ v) noexcept;

/// Calculates and returns the length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
length(VectorXZ v) noexcept;

/// Calculates and returns the squared length of a vector.
[[nodiscard]] POMDOG_EXPORT f32
lengthSquared(VectorXZ v) noexcept;

/// Calculates and returns the distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distance(VectorXZ a, VectorXZ b) noexcept;

/// Calculates and returns the squared distance between two vectors.
[[nodiscard]] POMDOG_EXPORT f32
distanceSquared(VectorXZ a, VectorXZ b) noexcept;

/// Calculates and returns the dot product of two vectors.
[[nodiscard]] POMDOG_EXPORT f32
dot(VectorXZ a, VectorXZ b) noexcept;

/// Returns the scalar cross product of two 2D vectors (result is signed area).
[[nodiscard]] POMDOG_EXPORT f32
cross(VectorXZ a, VectorXZ b) noexcept;

/// Returns a unit vector in the same direction from the specified vector.
[[nodiscard]] POMDOG_EXPORT VectorXZ
normalize(VectorXZ v) noexcept;

/// Returns a vector that contains the smallest components of two vectors.
[[nodiscard]] POMDOG_EXPORT VectorXZ
min(VectorXZ a, VectorXZ b) noexcept;

/// Returns a vector that contains the largest components of two vectors.
[[nodiscard]] POMDOG_EXPORT VectorXZ
max(VectorXZ a, VectorXZ b) noexcept;

/// Clamps the vector components to the [0, 1] range.
[[nodiscard]] POMDOG_EXPORT VectorXZ
saturate(VectorXZ v) noexcept;

/// Restricts a vector between a minimum and a maximum value.
[[nodiscard]] POMDOG_EXPORT VectorXZ
clamp(VectorXZ source, VectorXZ min, VectorXZ max) noexcept;

/// Performs a linear interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT VectorXZ
lerp(VectorXZ source1, VectorXZ source2, f32 amount) noexcept;

/// Performs Hermite interpolation between two vectors.
[[nodiscard]] POMDOG_EXPORT VectorXZ
smoothstep(VectorXZ source1, VectorXZ source2, f32 amount) noexcept;

/// Rotates a vector by a given angle in radians (counter-clockwise).
[[nodiscard]] POMDOG_EXPORT VectorXZ
rotate(VectorXZ v, Radian<f32> radian) noexcept;

/// Converts a vector to a positive angle relative to the +X axis.
[[nodiscard]] POMDOG_EXPORT Radian<f32>
toPositiveAngle(VectorXZ v) noexcept;

/// Converts a vector to a signed angle relative to the +X axis.
[[nodiscard]] POMDOG_EXPORT Radian<f32>
toSignedAngle(VectorXZ v) noexcept;

/// Converts a VectorXZ to a Vector2 type.
[[nodiscard]] POMDOG_EXPORT Vector2
toVector2(VectorXZ v) noexcept;

/// Converts a Vector2 to a VectorXZ type (interpreting y as z).
[[nodiscard]] POMDOG_EXPORT VectorXZ
toVectorXZ(Vector2 v) noexcept;

} // namespace pomdog::math
