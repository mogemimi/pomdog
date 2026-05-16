// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// Matrix2x2 is a 2x2 matrix in row-major order.
class POMDOG_EXPORT Matrix2x2 final {
public:
    /// The elements of the matrix.
    f32 m[2][2];

public:
    /// Creates a new Matrix2x2 object.
    Matrix2x2() noexcept;

    /// Constructs from floating-point values.
    Matrix2x2(f32 m00, f32 m01, f32 m10, f32 m11) noexcept;

    // Assignment operators:
    Matrix2x2& operator*=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator+=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator-=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator*=(f32 factor) noexcept;
    Matrix2x2& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix2x2 operator+() const noexcept;
    [[nodiscard]] Matrix2x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix2x2 operator+(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator-(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator*(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator*(f32 factor) const noexcept;
    [[nodiscard]] Matrix2x2 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix2x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix2x2& other) const noexcept;

    /// Returns the matrix element at the specified row and column.
    [[nodiscard]] f32 operator()(i32 row, i32 column) const noexcept;

    /// Returns a reference to the matrix element at the specified row and column.
    [[nodiscard]] f32& operator()(i32 row, i32 column) noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix2x2
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] POMDOG_EXPORT Matrix2x2
operator*(f32 factor, const Matrix2x2& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] POMDOG_EXPORT f32
determinant(const Matrix2x2& matrix) noexcept;

/// Multiplies a matrix by another matrix.
[[nodiscard]] POMDOG_EXPORT Matrix2x2
multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] POMDOG_EXPORT Matrix2x2
multiply(const Matrix2x2& matrix1, f32 factor) noexcept;

} // namespace pomdog::math
