// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_FLOATINGPOINTMATRIX2X2_D1549734_HPP
#define POMDOG_FLOATINGPOINTMATRIX2X2_D1549734_HPP

#include "ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include <array>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT FloatingPointMatrix2x2 {
public:
    static_assert(std::is_floating_point<T>::value, "T is floating point.");
    typedef T value_type;

    std::array<std::array<T, 2>, 2> m;

private:
    static constexpr std::size_t RowSize = 2;
    static constexpr std::size_t ColumnSize = 2;

public:
    // Constructors:
    FloatingPointMatrix2x2() noexcept = default;

    ///@brief Construct from floating-point values.
    FloatingPointMatrix2x2(T m00, T m01, T m10, T m11) noexcept;

    // Assignment operators:
    FloatingPointMatrix2x2 & operator*=(FloatingPointMatrix2x2 const& other);
    FloatingPointMatrix2x2 & operator+=(FloatingPointMatrix2x2 const& other);
    FloatingPointMatrix2x2 & operator-= (FloatingPointMatrix2x2 const& other);
    FloatingPointMatrix2x2 & operator*=(T scaleFactor);
    FloatingPointMatrix2x2 & operator/=(T scaleFactor);

    // Unary operators:
    FloatingPointMatrix2x2 operator+() const;
    FloatingPointMatrix2x2 operator-() const;

    // Binary operators:
    FloatingPointMatrix2x2 operator+(FloatingPointMatrix2x2 const& other) const;
    FloatingPointMatrix2x2 operator-(FloatingPointMatrix2x2 const& other) const;
    FloatingPointMatrix2x2 operator*(FloatingPointMatrix2x2 const& other) const;
    FloatingPointMatrix2x2 operator*(T scaleFactor) const;
    FloatingPointMatrix2x2 operator/(T scaleFactor) const;

    bool operator==(FloatingPointMatrix2x2 const& other) const;
    bool operator!=(FloatingPointMatrix2x2 const& other) const;

    // Function-call operators:
    T const& operator()(std::size_t row, std::size_t column) const;
    T & operator()(std::size_t row, std::size_t column);

    FloatingPointMatrix2x2 Concatenate(FloatingPointMatrix2x2 const& other) const;

    FloatingPointMatrix2x2 Concatenate(T scaleFactor) const;

    T Determinant() const;

    ///@brief Returns pointer to the first element.
    T const* Data() const noexcept;

    ///@brief Returns pointer to the first element.
    T* Data() noexcept;

    static FloatingPointMatrix2x2 const Identity;
};

template <typename T>
FloatingPointMatrix2x2<T> POMDOG_EXPORT
operator*(T scaleFactor, FloatingPointMatrix2x2<T> const& matrix);

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_FLOATINGPOINTMATRIX2X2_D1549734_HPP
