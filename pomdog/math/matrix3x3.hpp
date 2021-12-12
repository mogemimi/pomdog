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

/// Matrix3x3 is a 3x3 matrix in row-major order.
class POMDOG_EXPORT Matrix3x3 final {
public:
    float m[3][3];

private:
    static constexpr std::size_t RowSize = 3;
    static constexpr std::size_t ColumnSize = 3;

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
    Matrix3x3& operator*=(float scaleFactor) noexcept;
    Matrix3x3& operator/=(float scaleFactor);

    // Unary operators:
    Matrix3x3 operator+() const noexcept;
    Matrix3x3 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x3 operator+(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator-(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const noexcept;
    [[nodiscard]] Matrix3x3 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Matrix3x3 operator/(float scaleFactor) const;

    [[nodiscard]] bool operator==(const Matrix3x3& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x3& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column);

    void SetScale(const Vector3& scale) noexcept;

    [[nodiscard]] Vector3
    GetScale() const noexcept;

    [[nodiscard]] float Determinant() const noexcept;

    [[nodiscard]] Matrix2x2
    Minor2x2(std::size_t row, std::size_t column) const;

    [[nodiscard]] static Matrix3x3
    Multiply(const Matrix3x3& matrix1, const Matrix3x3& matrix2) noexcept;

    [[nodiscard]] static Matrix3x3
    Multiply(const Matrix3x3& matrix1, float scaleFactor) noexcept;

    [[nodiscard]] static Matrix3x3
    Adjoint(const Matrix3x3& matrix);

    static void
    Transpose(const Matrix3x3& matrix, Matrix3x3& result) noexcept;

    [[nodiscard]] static Matrix3x3
    Transpose(const Matrix3x3& matrix) noexcept;

    static void
    Invert(const Matrix3x3& matrix, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    Invert(const Matrix3x3& matrix);

    static void
    Lerp(const Matrix3x3& source1, const Matrix3x3& source2,
        float amount, Matrix3x3& result) noexcept;

    [[nodiscard]] static Matrix3x3
    Lerp(const Matrix3x3& source1, const Matrix3x3& source2,
        float amount) noexcept;

    static void
    CreateTranslation(const Vector2& position, Matrix3x3& result) noexcept;

    [[nodiscard]] static Matrix3x3
    CreateTranslation(const Vector2& position) noexcept;

    static void
    CreateScale(float scale, Matrix3x3& result) noexcept;

    [[nodiscard]] static Matrix3x3
    CreateScale(float scale) noexcept;

    static void
    CreateScale(const Vector3& scale, Matrix3x3& result) noexcept;

    [[nodiscard]] static Matrix3x3
    CreateScale(const Vector3& scale) noexcept;

    static void
    CreateRotationX(const Radian<float>& angle, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    CreateRotationX(const Radian<float>& angle);

    static void
    CreateRotationY(const Radian<float>& angle, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    CreateRotationY(const Radian<float>& angle);

    static void
    CreateRotationZ(const Radian<float>& angle, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    CreateRotationZ(const Radian<float>& angle);

    static void
    CreateFromQuaternion(const Quaternion& quaternion, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    CreateFromQuaternion(const Quaternion& quaternion);

    static void
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle, Matrix3x3& result);

    [[nodiscard]] static Matrix3x3
    CreateFromAxisAngle(const Vector3& axis, const Radian<float>& angle);

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    static Matrix3x3 const Identity;
};

[[nodiscard]] Matrix3x3 POMDOG_EXPORT
operator*(float scaleFactor, const Matrix3x3& matrix) noexcept;

} // namespace pomdog
