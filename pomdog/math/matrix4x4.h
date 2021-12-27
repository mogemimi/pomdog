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
    Matrix4x4 operator+() const noexcept;
    Matrix4x4 operator-() const noexcept;

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

    void SetTranslation(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    GetTranslation() const noexcept;

    void SetScale(const Vector3&) noexcept;

    [[nodiscard]] Vector3
    GetScale() const noexcept;

    [[nodiscard]] static Matrix4x4
    CreateTranslation(const Vector3& position) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateScale(float scale) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateScale(const Vector3& scale) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateRotationX(const Radian<float>& angle);

    [[nodiscard]] static Matrix4x4
    CreateRotationY(const Radian<float>& angle);

    [[nodiscard]] static Matrix4x4
    CreateRotationZ(const Radian<float>& angle);

    [[nodiscard]] static Matrix4x4
    CreateFromQuaternion(const Quaternion& quaternion);

    [[nodiscard]] static Matrix4x4
    CreateLookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up);

    [[nodiscard]] static Matrix4x4
    CreateLookAtRH(const Vector3& eye, const Vector3& at, const Vector3& up);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveLH(float width, float height, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveRH(float width, float height, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicRH(float width, float height, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    [[nodiscard]] static Matrix4x4
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    /// Returns the identity matrix.
    [[nodiscard]] static Matrix4x4 Identity() noexcept;
};

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
operator*(float factor, const Matrix4x4& matrix) noexcept;

} // namespace pomdog

namespace pomdog::math {

/// Calculates and returns the determinant of the matrix.
[[nodiscard]] float POMDOG_EXPORT
Determinant(const Matrix4x4& matrix) noexcept;

/// Returns the minor of a matrix.
[[nodiscard]] Matrix3x3 POMDOG_EXPORT
Minor3x3(const Matrix4x4& matrix, std::size_t row, std::size_t column);

/// Multiplies a matrix by another matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) noexcept;

/// Multiplies a matrix by a scalar factor.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Multiply(const Matrix4x4& matrix1, float factor) noexcept;

/// Calculates and returns the adjoint matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Adjoint(const Matrix4x4& matrix);

/// Returns a matrix that transposes the rows and columns of the input matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Transpose(const Matrix4x4& matrix) noexcept;

/// Calculates and returns the inverse of a matrix.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Invert(const Matrix4x4& matrix);

/// Performs a linear interpolation between two matrices.
[[nodiscard]] Matrix4x4 POMDOG_EXPORT
Lerp(const Matrix4x4& source1, const Matrix4x4& source2, float amount) noexcept;

} // namespace pomdog::math
