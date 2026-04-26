// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Vector2;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Matrix3x2 is a 3x2 matrix in row-major order.
class POMDOG_EXPORT Matrix3x2 final {
public:
    /// The elements of the matrix.
    f32 m[3][2];

public:
    /// Creates a new Matrix3x2 object.
    Matrix3x2() noexcept;

    /// Constructs from floating-point values.
    Matrix3x2(
        f32 m00, f32 m01,
        f32 m10, f32 m11,
        f32 m20, f32 m21) noexcept;

    // Assignment operators:
    Matrix3x2& operator*=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator+=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator-=(const Matrix3x2& other) noexcept;
    Matrix3x2& operator*=(f32 factor) noexcept;
    Matrix3x2& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix3x2 operator+() const noexcept;
    [[nodiscard]] Matrix3x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x2 operator+(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator-(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(f32 factor) const noexcept;
    [[nodiscard]] Matrix3x2 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix3x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x2& other) const noexcept;

    /// Returns the matrix element at the specified row and column.
    [[nodiscard]] f32 operator()(i32 row, i32 column) const noexcept;

    /// Returns a reference to the matrix element at the specified row and column.
    [[nodiscard]] f32& operator()(i32 row, i32 column) noexcept;

    /// Creates a translation matrix.
    [[nodiscard]] static Matrix3x2
    createTranslation(const Vector2& position) noexcept;

    /// Creates a scale matrix.
    [[nodiscard]] static Matrix3x2
    createScale(f32 scale) noexcept;

    /// Creates a scale matrix.
    [[nodiscard]] static Matrix3x2
    createScale(const Vector2& scale) noexcept;

    /// Creates a rotation matrix.
    [[nodiscard]] static Matrix3x2
    createRotation(const Radian<f32>& angle);

    /// Creates a skew matrix.
    [[nodiscard]] static Matrix3x2
    createSkew(const Vector2& skew);

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix3x2
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
operator*(f32 factor, const Matrix3x2& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] f32 POMDOG_EXPORT
determinant(const Matrix3x2& matrix) noexcept;

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
multiply(const Matrix3x2& matrix1, const Matrix3x2& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
multiply(const Matrix3x2& matrix1, f32 factor) noexcept;

/// Calculates and returns the inverse of a matrix.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
invert(const Matrix3x2& matrix);

/// Performs a linear interpolation between two matrices.
[[nodiscard]] Matrix3x2 POMDOG_EXPORT
lerp(const Matrix3x2& source1, const Matrix3x2& source2, f32 amount) noexcept;

} // namespace pomdog::math
