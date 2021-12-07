// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

/// FloatingPointMatrix2x2 is a 2x2 matrix in row-major order.
template <typename T>
class POMDOG_EXPORT FloatingPointMatrix2x2 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    using value_type = T;

    T m[2][2];

private:
    static constexpr std::size_t RowSize = 2;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    FloatingPointMatrix2x2() noexcept = default;

    /// Constructs from floating-point values.
    FloatingPointMatrix2x2(T m00, T m01, T m10, T m11) noexcept;

    // Assignment operators:
    FloatingPointMatrix2x2& operator*=(const FloatingPointMatrix2x2& other) noexcept;
    FloatingPointMatrix2x2& operator+=(const FloatingPointMatrix2x2& other) noexcept;
    FloatingPointMatrix2x2& operator-=(const FloatingPointMatrix2x2& other) noexcept;
    FloatingPointMatrix2x2& operator*=(T scaleFactor) noexcept;
    FloatingPointMatrix2x2& operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix2x2 operator+() const noexcept;
    FloatingPointMatrix2x2 operator-() const noexcept;

    // Binary operators:
    [[nodiscard]] FloatingPointMatrix2x2 operator+(const FloatingPointMatrix2x2& other) const noexcept;
    [[nodiscard]] FloatingPointMatrix2x2 operator-(const FloatingPointMatrix2x2& other) const noexcept;
    [[nodiscard]] FloatingPointMatrix2x2 operator*(const FloatingPointMatrix2x2& other) const noexcept;
    [[nodiscard]] FloatingPointMatrix2x2 operator*(T scaleFactor) const noexcept;
    [[nodiscard]] FloatingPointMatrix2x2 operator/(T scaleFactor) const;

    [[nodiscard]] bool operator==(const FloatingPointMatrix2x2& other) const noexcept;
    [[nodiscard]] bool operator!=(const FloatingPointMatrix2x2& other) const noexcept;

    // Function-call operators:
    [[nodiscard]] const T& operator()(std::size_t row, std::size_t column) const;
    [[nodiscard]] T& operator()(std::size_t row, std::size_t column);

    [[nodiscard]] T Determinant() const noexcept;

    [[nodiscard]] static FloatingPointMatrix2x2
    Multiply(const FloatingPointMatrix2x2& matrix1, const FloatingPointMatrix2x2& matrix2) noexcept;

    [[nodiscard]] static FloatingPointMatrix2x2
    Multiply(const FloatingPointMatrix2x2& matrix1, T scaleFactor) noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] const T* Data() const noexcept;

    /// Returns pointer to the first element.
    [[nodiscard]] T* Data() noexcept;

    static FloatingPointMatrix2x2 const Identity;
};

template <typename T>
FloatingPointMatrix2x2<T> const FloatingPointMatrix2x2<T>::Identity = {
    1, 0,
    0, 1,
};

template <typename T>
FloatingPointMatrix2x2<T> POMDOG_EXPORT
operator*(T scaleFactor, const FloatingPointMatrix2x2<T>& matrix) noexcept;

} // namespace Pomdog::Detail
