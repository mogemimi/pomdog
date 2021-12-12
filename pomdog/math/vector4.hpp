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

/// Vector4 is a vector in 4-dimensional space.
class POMDOG_EXPORT Vector4 final {
public:
    float X;
    float Y;
    float Z;
    float W;

public:
    /// Creates a new Vector4 object.
    Vector4() noexcept;

    /// Constructs from floating-point values.
    Vector4(float x, float y, float z, float w) noexcept;
    Vector4(const Vector3& xyz, float w) noexcept;

    // Assignment operators:
    Vector4& operator+=(const Vector4& other) noexcept;
    Vector4& operator-=(const Vector4& other) noexcept;
    Vector4& operator*=(float scaleFactor) noexcept;
    Vector4& operator/=(float scaleFactor);

    // Unary operators:
    Vector4 operator+() const noexcept;
    Vector4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Vector4 operator+(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator-(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator*(const Vector4& other) const noexcept;
    [[nodiscard]] Vector4 operator/(const Vector4& other) const;
    [[nodiscard]] Vector4 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Vector4 operator/(float scaleFactor) const;

    [[nodiscard]] bool operator==(const Vector4& other) const noexcept;
    [[nodiscard]] bool operator!=(const Vector4& other) const noexcept;

    [[nodiscard]] float Length() const noexcept;

    [[nodiscard]] float LengthSquared() const noexcept;

    [[nodiscard]] static float
    Distance(const Vector4& a, const Vector4& b) noexcept;

    [[nodiscard]] static float
    DistanceSquared(const Vector4& a, const Vector4& b) noexcept;

    [[nodiscard]] static float
    Dot(const Vector4& a, const Vector4& b) noexcept;

    void Normalize() noexcept;

    [[nodiscard]] static Vector4
    Normalize(const Vector4& source) noexcept;

    static void Normalize(const Vector4& source, Vector4& result) noexcept;

    [[nodiscard]] static Vector4
    Transform(
        const Vector4& position,
        const Matrix4x4& matrix) noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;
};

[[nodiscard]] Vector4 POMDOG_EXPORT
operator*(float scaleFactor, const Vector4& vector) noexcept;

} // namespace pomdog
