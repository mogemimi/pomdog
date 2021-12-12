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

/// Matrix4x4 is a 4x4 matrix in row-major order.
class POMDOG_EXPORT Matrix4x4 final {
public:
    float m[4][4];

private:
    static constexpr std::size_t RowSize = 4;
    static constexpr std::size_t ColumnSize = 4;

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
    Matrix4x4& operator*=(float scaleFactor) noexcept;
    Matrix4x4& operator/=(float scaleFactor) noexcept;

    // Unary operators:
    Matrix4x4 operator+() const noexcept;
    Matrix4x4 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix4x4 operator+(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator-(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4&) const noexcept;
    [[nodiscard]] Matrix4x4 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Matrix4x4 operator/(float scaleFactor) const noexcept;

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

    [[nodiscard]] float Determinant() const noexcept;

    [[nodiscard]] Matrix3x3
    Minor3x3(std::size_t row, std::size_t column) const;

    [[nodiscard]] static Matrix4x4
    Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2) noexcept;

    [[nodiscard]] static Matrix4x4
    Multiply(const Matrix4x4& matrix1, float scaleFactor) noexcept;

    [[nodiscard]] static Matrix4x4
    Adjoint(const Matrix4x4& matrix);

    static void
    Transpose(const Matrix4x4& matrix, Matrix4x4& result) noexcept;

    [[nodiscard]] static Matrix4x4
    Transpose(const Matrix4x4& matrix) noexcept;

    static void
    Invert(const Matrix4x4& matrix, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    Invert(const Matrix4x4& matrix);

    static void
    Lerp(const Matrix4x4& source1, const Matrix4x4& source2, float amount,
        Matrix4x4& result) noexcept;

    [[nodiscard]] static Matrix4x4
    Lerp(const Matrix4x4& source1, const Matrix4x4& source2, float amount) noexcept;

    static void
    CreateTranslation(const Vector3& position, Matrix4x4& result) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateTranslation(const Vector3& position) noexcept;

    static void
    CreateScale(float scale, Matrix4x4& result) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateScale(float scale) noexcept;

    static void
    CreateScale(const Vector3& scale, Matrix4x4& result) noexcept;

    [[nodiscard]] static Matrix4x4
    CreateScale(const Vector3& scale) noexcept;

    static void
    CreateRotationX(const Radian<float>& angle, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateRotationX(const Radian<float>& angle);

    static void
    CreateRotationY(const Radian<float>& angle, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateRotationY(const Radian<float>& angle);

    static void
    CreateRotationZ(const Radian<float>& angle, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateRotationZ(const Radian<float>& angle);

    static void
    CreateFromQuaternion(const Quaternion& quaternion, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateFromQuaternion(const Quaternion& quaternion);

    static void
    CreateLookAtLH(const Vector3& eye, const Vector3& at,
        const Vector3& up, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateLookAtLH(const Vector3& eye, const Vector3& at,
        const Vector3& up);

    static void
    CreateLookAtRH(const Vector3& eye, const Vector3& at,
        const Vector3& up, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateLookAtRH(const Vector3& eye, const Vector3& at,
        const Vector3& up);

    static void
    CreatePerspectiveLH(float width, float height, float zNear, float zFar, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveLH(float width, float height, float zNear, float zFar);

    static void
    CreatePerspectiveRH(float width, float height, float zNear, float zFar, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveRH(float width, float height, float zNear, float zFar);

    static void
    CreatePerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveFieldOfViewLH(const Radian<float>& fovy, float aspect, float zNear, float zFar);

    static void
    CreatePerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveFieldOfViewRH(const Radian<float>& fovy, float aspect, float zNear, float zFar);

    static void
    CreatePerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveOffCenterLH(float left, float right, float bottom, float top, float zNear, float zFar);

    static void
    CreatePerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreatePerspectiveOffCenterRH(float left, float right, float bottom, float top, float zNear, float zFar);

    static void
    CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicLH(float width, float height, float zNearPlane, float zFarPlane);

    static void
    CreateOrthographicRH(float width, float height, float zNearPlane, float zFarPlane,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicRH(float width, float height, float zNearPlane, float zFarPlane);

    static void
    CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicOffCenterLH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    static void
    CreateOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane,
        Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateOrthographicOffCenterRH(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

    static void
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle, Matrix4x4& result);

    [[nodiscard]] static Matrix4x4
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    static Matrix4x4 const Identity;
};

[[nodiscard]] Matrix4x4 POMDOG_EXPORT
operator*(float scaleFactor, const Matrix4x4& matrix) noexcept;

} // namespace pomdog
