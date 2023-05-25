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

/// Matrix3x3 is a 3x3 matrix in row-major order.
class POMDOG_EXPORT Matrix3x3 final {
public:
    float m[3][3];

public:
    // Constructors:
    Matrix3x3() noexcept;

    /// Constructs from floating-point values.
    Matrix3x3(
        float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22) noexcept;

    // Assignment operators:
    Matrix3x3& operator*=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator+=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator-=(const Matrix3x3& other) noexcept;
    Matrix3x3& operator*=(float factor) noexcept;
    Matrix3x3& operator/=(float factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix3x3 operator+() const noexcept;
    [[nodiscard]] Matrix3x3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x3 operator+(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator-(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(float factor) const noexcept;
    [[nodiscard]] Matrix3x3 operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix3x3& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x3& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const noexcept;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column) noexcept;

    void setScale(const Vector3& scale) noexcept;

    [[nodiscard]] Vector3
    getScale() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const float* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* data() noexcept;

    [[nodiscard]] static Matrix3x3
    createTranslation(const Vector2& position) noexcept;

    [[nodiscard]] static Matrix3x3
    createScale(float scale) noexcept;

    [[nodiscard]] static Matrix3x3
    createScale(const Vector3& scale) noexcept;

    [[nodiscard]] static Matrix3x3
    createRotationX(const Radian<float>& angle);

    [[nodiscard]] static Matrix3x3
    createRotationY(const Radian<float>& angle);

    [[nodiscard]] static Matrix3x3
    createRotationZ(const Radian<float>& angle);

    [[nodiscard]] static Matrix3x3
    createFromQuaternion(const Quaternion& quaternion);

    [[nodiscard]] static Matrix3x3
    createFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix3x3
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
operator*(float factor, const Matrix3x3& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] float POMDOG_EXPORT
determinant(const Matrix3x3& matrix) noexcept;

/// Returns the minor of a matrix.
[[nodiscard]] Matrix2x2 POMDOG_EXPORT
minor2x2(const Matrix3x3& matrix, std::size_t row, std::size_t column);

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
multiply(const Matrix3x3& matrix1, float factor) noexcept;

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
lerp(const Matrix3x3& source1, const Matrix3x3& source2, float amount) noexcept;

} // namespace pomdog::math
