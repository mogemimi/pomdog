// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {
class Matrix3x3;
class Vector3;
class Quaternion;
template <typename T>
class Radian;
} // namespace pomdog

namespace pomdog {

/// Matrix4x4 is a 4x4 matrix in row-major order.
class POMDOG_EXPORT Matrix4x4 final {
public:
    f32 m[4][4];

public:
    // Constructors:
    Matrix4x4() noexcept;

    /// Constructs from floating-point values.
    Matrix4x4(
        f32 m00, f32 m01, f32 m02, f32 m03,
        f32 m10, f32 m11, f32 m12, f32 m13,
        f32 m20, f32 m21, f32 m22, f32 m23,
        f32 m30, f32 m31, f32 m32, f32 m33) noexcept;

    // Assignment operators:
    Matrix4x4& operator*=(const Matrix4x4&) noexcept;
    Matrix4x4& operator+=(const Matrix4x4&) noexcept;
    Matrix4x4& operator-=(const Matrix4x4&) noexcept;
    Matrix4x4& operator*=(f32 factor) noexcept;
    Matrix4x4& operator/=(f32 factor) noexcept;

    // Unary operators:
    [[nodiscard]] Matrix4x4 operator+() const noexcept;
    [[nodiscard]] Matrix4x4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix4x4 operator+(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator-(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(f32 factor) const noexcept;
    [[nodiscard]] Matrix4x4 operator/(f32 factor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix4x4&) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix4x4&) const noexcept;

    // Function-call operators:
    [[nodiscard]] f32 operator()(i32 row, i32 column) const noexcept;
    [[nodiscard]] f32& operator()(i32 row, i32 column) noexcept;

    void setTranslation(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    getTranslation() const noexcept;

    void setScale(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    getScale() const noexcept;

    [[nodiscard]] static Matrix4x4
    createTranslation(const Vector3& position) noexcept;

    [[nodiscard]] static Matrix4x4
    createScale(f32 scale) noexcept;

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
    createPerspectiveLH(f32 width, f32 height, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveRH(f32 width, f32 height, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveFieldOfViewLH(const Radian<f32>& fovy, f32 aspect, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveFieldOfViewRH(const Radian<f32>& fovy, f32 aspect, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveOffCenterLH(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createPerspectiveOffCenterRH(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);

    [[nodiscard]] static Matrix4x4
    createOrthographicLH(f32 width, f32 height, f32 zNearPlane, f32 zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicRH(f32 width, f32 height, f32 zNearPlane, f32 zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicOffCenterLH(f32 left, f32 right, f32 bottom, f32 top, f32 zNearPlane, f32 zFarPlane);

    [[nodiscard]] static Matrix4x4
    createOrthographicOffCenterRH(f32 left, f32 right, f32 bottom, f32 top, f32 zNearPlane, f32 zFarPlane);

    [[nodiscard]] static Matrix4x4
    createFromAxisAngle(const Vector3& axis, const Radian<f32>& angle);

    /// Returns pointer to the first element.
    [[nodiscard]] const f32* data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] f32* data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix4x4
    createIdentity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
operator*(f32 factor, const Matrix4x4& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] f32 POMDOG_EXPORT
determinant(const Matrix4x4& matrix) noexcept;

/// Returns the minor of a matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
minor3x3(const Matrix4x4& matrix, i32 row, i32 column);

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
multiply(const Matrix4x4& matrix1, f32 factor) noexcept;

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
lerp(const Matrix4x4& source1, const Matrix4x4& source2, f32 amount) noexcept;

} // namespace pomdog::math
