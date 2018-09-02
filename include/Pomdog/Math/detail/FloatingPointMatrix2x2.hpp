// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointMatrix2x2 final {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    T m[2][2];

private:
    static constexpr std::size_t RowSize = 2;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    FloatingPointMatrix2x2() noexcept = default;

    ///@brief Construct from floating-point values.
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
    FloatingPointMatrix2x2 operator+(const FloatingPointMatrix2x2& other) const noexcept;
    FloatingPointMatrix2x2 operator-(const FloatingPointMatrix2x2& other) const noexcept;
    FloatingPointMatrix2x2 operator*(const FloatingPointMatrix2x2& other) const noexcept;
    FloatingPointMatrix2x2 operator*(T scaleFactor) const noexcept;
    FloatingPointMatrix2x2 operator/(T scaleFactor) const;

    bool operator==(const FloatingPointMatrix2x2& other) const noexcept;
    bool operator!=(const FloatingPointMatrix2x2& other) const noexcept;

    // Function-call operators:
    const T& operator()(std::size_t row, std::size_t column) const;
    T& operator()(std::size_t row, std::size_t column);

    T Determinant() const noexcept;

    static FloatingPointMatrix2x2
    Multiply(const FloatingPointMatrix2x2& matrix1, const FloatingPointMatrix2x2& matrix2) noexcept;

    static FloatingPointMatrix2x2
    Multiply(const FloatingPointMatrix2x2& matrix1, T scaleFactor) noexcept;

    ///@brief Returns pointer to the first element.
    const T* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

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

} // namespace Detail
} // namespace Pomdog
