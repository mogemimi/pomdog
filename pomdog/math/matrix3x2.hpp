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

/// Matrix3x2 is a 3x2 matrix in row-major order.
class POMDOG_EXPORT Matrix3x2 final {
public:
    float m[3][2];

private:
    static constexpr std::size_t RowSize = 3;
    static constexpr std::size_t ColumnSize = 2;

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
    Matrix3x2& operator*=(float scaleFactor) noexcept;
    Matrix3x2& operator/=(float scaleFactor) noexcept;

    // Unary operators:
    Matrix3x2 operator+() const noexcept;
    Matrix3x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix3x2 operator+(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator-(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(const Matrix3x2& other) const noexcept;
    [[nodiscard]] Matrix3x2 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Matrix3x2 operator/(float scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix3x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix3x2& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const noexcept;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column) noexcept;

    [[nodiscard]] float Determinant() const noexcept;

    [[nodiscard]] static Matrix3x2
    Multiply(const Matrix3x2& matrix1, const Matrix3x2& matrix2) noexcept;

    [[nodiscard]] static Matrix3x2
    Multiply(const Matrix3x2& matrix1, float scaleFactor) noexcept;

    static void
    Invert(const Matrix3x2& matrix, Matrix3x2& result);

    [[nodiscard]] static Matrix3x2
    Invert(const Matrix3x2& matrix);

    static void
    Lerp(const Matrix3x2& source1, const Matrix3x2& source2,
        float amount, Matrix3x2& result) noexcept;

    [[nodiscard]] static Matrix3x2
    Lerp(const Matrix3x2& source1, const Matrix3x2& source2,
        float amount) noexcept;

    static void
    CreateTranslation(const Vector2& position, Matrix3x2& result) noexcept;

    [[nodiscard]] static Matrix3x2
    CreateTranslation(const Vector2& position) noexcept;

    static void
    CreateScale(float scale, Matrix3x2& result) noexcept;

    [[nodiscard]] static Matrix3x2
    CreateScale(float scale) noexcept;

    static void
    CreateScale(const Vector2& scale, Matrix3x2& result) noexcept;

    [[nodiscard]] static Matrix3x2
    CreateScale(const Vector2& scale) noexcept;

    static void
    CreateRotation(const Radian<float>& angle, Matrix3x2& result);

    [[nodiscard]] static Matrix3x2
    CreateRotation(const Radian<float>& angle);

    static void
    CreateSkew(const Vector2& skew, Matrix3x2& result);

    [[nodiscard]] static Matrix3x2
    CreateSkew(const Vector2& skew);

    /// Returns pointer to the first element.
    const float* Data() const noexcept;

    /// Returns pointer to the first element.
    float* Data() noexcept;

    static Matrix3x2 const Identity;
};

[[nodiscard]] Matrix3x2 POMDOG_EXPORT
operator*(float scaleFactor, const Matrix3x2& matrix) noexcept;

} // namespace pomdog
