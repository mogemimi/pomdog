// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Matrix2x2;
class Vector2;
class Vector3;
class Quaternion;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Matrix3x3 is a 3x3 matrix in row-major order.
class POMDOG_EXPORT Matrix3x3 final {
public:
    f32 m[3][3];

public:
    // Constructors:
    Matrix3x3() noexcept;

    /// Constructs from floating-point values.
    Matrix3x3(
        f32 m00, f32 m01, f32 m02,
        f32 m10, f32 m11, f32 m12,
        f32 m20, f32 m21, f32 m22) noexcept;

    // Assignment operators:
    Matrix3x3& operator*=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator+=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator-=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator*=(f32 factor) noexcept;
    Matrix3x3& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix3x3 operator+() const noexcept;
    [[nodiscard]] Matrix3x3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x3 operator+(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator-(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(f32 factor) const noexcept;
    [[nodiscard]] Matrix3x3 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix3x3& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x3& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] f32 operator()(i32 row, i32 column) const noexcept;
    [[nodiscard]] f32& operator()(i32 row, i32 column) noexcept;

    void setScale(const Vector3& scale) noexcept;

    [[nodiscard]] Vector3
    getScale() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    [[nodiscard]] static Matrix3x3
    createTranslation(const Vector2& position) noexcept;

    [[nodiscard]] static Matrix3x3
    createScale(f32 scale) noexcept;

    [[nodiscard]] static Matrix3x3
    createScale(const Vector3& scale) noexcept;

    [[nodiscard]] static Matrix3x3
    createRotationX(const Radian<f32>& angle);

    [[nodiscard]] static Matrix3x3
    createRotationY(const Radian<f32>& angle);

    [[nodiscard]] static Matrix3x3
    createRotationZ(const Radian<f32>& angle);

    [[nodiscard]] static Matrix3x3
    createFromQuaternion(const Quaternion& quaternion);

    [[nodiscard]] static Matrix3x3
    createFromAxisAngle(const Vector3& axis, const Radian<f32>& angle);

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix3x3
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
operator*(f32 factor, const Matrix3x3& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] f32 POMDOG_EXPORT
determinant(const Matrix3x3& matrix) noexcept;

/// Returns the minor of a matrix.
[[nodiscard]] Matrix2x2 POMDOG_EXPORT
minor2x2(const Matrix3x3& matrix, i32 row, i32 column);

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
multiply(const Matrix3x3& matrix1, f32 factor) noexcept;

/// Calculates and returns the adjoint matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
adjoint(const Matrix3x3& matrix);

/// Returns a matrix that transposes the rows and columns of the input matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
transpose(const Matrix3x3& matrix) noexcept;

/// Calculates and returns the inverse of a matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
invert(const Matrix3x3& matrix);

/// Performs a linear interpolation between two matrices.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
lerp(const Matrix3x3& source1, const Matrix3x3& source2, f32 amount) noexcept;

} // namespace pomdog::math
