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

    [[nodiscard]] Vector2 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Vector2 operator/(float scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const Vector2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector2& other) const noexcept;

    [[nodiscard]] float Length() const noexcept;

    [[nodiscard]] float LengthSquared() const noexcept;

    [[nodiscard]] static float
    Distance(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static float
    DistanceSquared(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static float
    Dot(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static float
    Cross(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static Vector2
    Min(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static Vector2
    Max(const Vector2& a, const Vector2& b) noexcept;

    [[nodiscard]] static Vector2
    Clamp(
        const Vector2& source,
        const Vector2& min,
        const Vector2& max) noexcept;

    void Normalize() noexcept;

    [[nodiscard]] static Vector2
    Normalize(const Vector2& source) noexcept;

    static void
    Normalize(const Vector2& source, Vector2& result) noexcept;

    /// Rotates a vector by a given angle in radians.
    [[nodiscard]] static Vector2
    Rotate(const Vector2& vector, const Radian<float>& radian) noexcept;

    [[nodiscard]] static Vector2
    Transform(
        const Vector2& position,
        const Matrix3x2& matrix) noexcept;

    [[nodiscard]] static Vector2
    Transform(
        const Vector2& position,
        const Matrix4x4& matrix) noexcept;

    [[nodiscard]] static Vector2
    Transform(
        const Vector2& position,
        const Quaternion& rotation) noexcept;

    [[nodiscard]] static Vector2
    TransformNormal(
        const Vector2& normal,
        const Matrix4x4& matrix) noexcept;

    [[nodiscard]] static Vector2
    Lerp(
        const Vector2& source1,
        const Vector2& source2,
        float amount);

    [[nodiscard]] static Vector2
    SmoothStep(
        const Vector2& source1,
        const Vector2& source2,
        float amount);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    ///@note (0, 0)
    static Vector2 const Zero;
};

[[nodiscard]] Vector2 POMDOG_EXPORT
operator*(float scaleFactor, const Vector2& vector) noexcept;

} // namespace pomdog
