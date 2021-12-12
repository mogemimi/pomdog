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

/// Matrix2x2 is a 2x2 matrix in row-major order.
class POMDOG_EXPORT Matrix2x2 final {
public:
    float m[2][2];

private:
    static constexpr std::size_t RowSize = 2;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    Matrix2x2() noexcept;

    /// Constructs from floating-point values.
    Matrix2x2(float m00, float m01, float m10, float m11) noexcept;

    // Assignment operators:
    Matrix2x2& operator*=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator+=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator-=(const Matrix2x2& other) noexcept;
    Matrix2x2& operator*=(float scaleFactor) noexcept;
    Matrix2x2& operator/=(float scaleFactor) noexcept;

    // Unary operators:
    Matrix2x2 operator+() const noexcept;
    Matrix2x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] Matrix2x2 operator+(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator-(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator*(const Matrix2x2& other) const noexcept;
    [[nodiscard]] Matrix2x2 operator*(float scaleFactor) const noexcept;
    [[nodiscard]] Matrix2x2 operator/(float scaleFactor) const noexcept;

    [[nodiscard]] bool operator==(const Matrix2x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const Matrix2x2& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const float& operator()(std::size_t row, std::size_t column) const noexcept;
    [[nodiscard]] float& operator()(std::size_t row, std::size_t column) noexcept;

    [[nodiscard]] float Determinant() const noexcept;

    [[nodiscard]] static Matrix2x2
    Multiply(const Matrix2x2& matrix1, const Matrix2x2& matrix2) noexcept;

    [[nodiscard]] static Matrix2x2
    Multiply(const Matrix2x2& matrix1, float scaleFactor) noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const float* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] float* Data() noexcept;

    static Matrix2x2 const Identity;
};

[[nodiscard]] Matrix2x2 POMDOG_EXPORT
operator*(float scaleFactor, const Matrix2x2& matrix) noexcept;

} // namespace pomdog
