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

/// Matrix4x4 is a 4x4 matrix in row-major order.
class POMDOG_EXPORT Matrix4x4 final {
public:
    float m[4][4];

public:
    // Constructors:
    Matrix4x4() noexcept;

    /// Constructs from floating-point values.
    Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept;

    // Assignment operators:
    Matrix4x4& operator*=(const Matrix4x4&) noexcept;
    Matrix4x4& operator+=(const Matrix4x4&) noexcept;
    Matrix4x4& operator-=(const Matrix4x4&) noexcept;
    Matrix4x4& operator*=(float factor) noexcept;
    Matrix4x4& operator/=(float factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix4x4 operator+() const noexcept;
    [[nodiscard]] Matrix4x4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix4x4 operator+(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator-(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(float factor) const noexcept;
    [[nodiscard]] Matrix4x4 operator/(float factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix4x4&) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix4x4&) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const noexcept;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column) noexcept;

    void setTranslation(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    getTranslation() const noexcept;

    void setScale(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    getScale() const noexcept;

    [[nodiscard]] static Matrix4x4
    createTranslation(const Vector3& position) noexcept;

    [[nodiscard]] static Matrix4x4
    createScale(float scale) noexcept;

    [[nodiscard]] static Matrix4x4
    createScale(const Vector3& scale) noexcept;

    [[nodiscard]] static Matrix4x4
    createRotationX(const Radian<f32>& angle);

    [[nodiscard]] static Matrix4x4
    createRotationY(const Radian<f32>& angle);

    [[nodiscard]] static Matrix4x4
    createRotationZ(const Radian<f32>& angle);

    [[nodiscard]] static Matrix4x4
    createFromQuaternion(const Quaternion& quaternion);

    [[nodiscard]] static Matrix4x4
    createLookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up);

    [[nodiscard]] static Matrix4x4
    createLookAtRH(const Vector3& eye, const Vector3& at, const Vector3& up);

    [[nodiscard]] static Matrix4x4
    createPerspectiveLH(float width, float height, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveRH(float width, float height, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveFieldOfViewLH(const Radian<f32>& fovy, float aspect, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveFieldOfViewRH(const Radian<f32>& fovy, float aspect, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    createOrthographicLH(float width, float height, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicRH(float width, float height, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    createFromAxisAngle(const Vector3& axis, const Radian<f32>& angle);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix4x4
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
operator*(float factor, const Matrix4x4& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] float POMDOG_EXPORT
determinant(const Matrix4x4& matrix) noexcept;

/// Returns the minor of a matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
minor3x3(const Matrix4x4& matrix, std::size_t row, std::size_t column);

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
multiply(const Matrix4x4& matrix1, float factor) noexcept;

/// Calculates and returns the adjoint matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
adjoint(const Matrix4x4& matrix);

/// Returns a matrix that transposes the rows and columns of the input matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
transpose(const Matrix4x4& matrix) noexcept;

/// Calculates and returns the inverse of a matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
invert(const Matrix4x4& matrix);

/// Performs a linear interpolation between two matrices.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
lerp(const Matrix4x4& source1, const Matrix4x4& source2, float amount) noexcept;

} // namespace pomdog::math
