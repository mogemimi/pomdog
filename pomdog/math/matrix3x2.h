// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Matrix3x2 is a 3x2 matrix in row-major order.
class POMDOG_EXPORT Matrix3x2 final {
public:
    float m[3][2];

public:
    // Constructors:
    Matrix3x2() noexcept;

    /// Constructs from floating-point values.
    Matrix3x2(
        float m00, float m01,
        float m10, float m11,
        float m20, float m21) noexcept;

    // Assignment operators:
    Matrix3x2& operator*=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator+=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator-=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator*=(float factor) noexcept;
    Matrix3x2& operator/=(float factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix3x2 operator+() const noexcept;
    [[nodiscard]] Matrix3x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x2 operator+(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator-(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(float factor) const noexcept;
    [[nodiscard]] Matrix3x2 operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix3x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x2& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const noexcept;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column) noexcept;

    [[nodiscard]] static Matrix3x2
    createTranslation(const Vector2& position) noexcept;

    [[nodiscard]] static Matrix3x2
    createScale(float scale) noexcept;

    [[nodiscard]] static Matrix3x2
    createScale(const Vector2& scale) noexcept;

    [[nodiscard]] static Matrix3x2
    createRotation(const Radian<float>& angle);

    [[nodiscard]] static Matrix3x2
    createSkew(const Vector2& skew);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix3x2
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
operator*(float factor, const Matrix3x2& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] float POMDOG_EXPORT
determinant(const Matrix3x2& matrix) noexcept;

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
multiply(const Matrix3x2& matrix1, const Matrix3x2& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
multiply(const Matrix3x2& matrix1, float factor) noexcept;

/// Calculates and returns the inverse of a matrix.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
invert(const Matrix3x2& matrix);

/// Performs a linear interpolation between two matrices.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
lerp(const Matrix3x2& source1, const Matrix3x2& source2, float amount) noexcept;

} // namespace pomdog::math
