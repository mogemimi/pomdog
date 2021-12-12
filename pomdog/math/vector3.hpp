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

/// Vector3 is a vector in 3-dimensional space.
class POMDOG_EXPORT Vector3 final {
public:
    float X;
    float Y;
    float Z;

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
    Vector3 operator+() const noexcept;
    Vector3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector3 operator+(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator-(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator*(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator/(const Vector3&) const noexcept;
    [[nodiscard]] Vector3 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Vector3 operator/(float scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const Vector3& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector3& other) const noexcept;

    [[nodiscard]] float Length() const noexcept;

    [[nodiscard]] float LengthSquared() const noexcept;

    [[nodiscard]] static float
    Distance(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static float
    DistanceSquared(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static float
    Dot(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static Vector3
    Cross(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static Vector3
    Min(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static Vector3
    Max(const Vector3& a, const Vector3& b) noexcept;

    [[nodiscard]] static Vector3
    Clamp(
        const Vector3& source,
        const Vector3& min,
        const Vector3& max) noexcept;

    [[nodiscard]] static Vector3
    Lerp(
        const Vector3& source1,
        const Vector3& source2,
        float amount);

    [[nodiscard]] static Vector3
    SmoothStep(
        const Vector3& source1,
        const Vector3& source2,
        float amount);

    void Normalize() noexcept;

    [[nodiscard]] static Vector3
    Normalize(const Vector3& source) noexcept;

    static void
    Normalize(const Vector3&, Vector3& result) noexcept;

    [[nodiscard]] static Vector3
    Transform(
        const Vector3& position,
        const Matrix4x4& matrix) noexcept;

    [[nodiscard]] static Vector3
    TransformNormal(
        const Vector3& normal,
        const Matrix4x4& matrix) noexcept;

    [[nodiscard]] static Vector3
    ToEulerAngles(const Quaternion& quaternion);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    ///@note (0, 0, 0)
    static Vector3 const Zero;

    ///@note (1, 0, 0)
    static Vector3 const UnitX;

    ///@note (0, 1, 0)
    static Vector3 const UnitY;

    ///@note (0, 0, 1)
    static Vector3 const UnitZ;
};

[[nodiscard]] Vector3 POMDOG_EXPORT
operator*(float scaleFactor, const Vector3& vector) noexcept;

} // namespace pomdog
