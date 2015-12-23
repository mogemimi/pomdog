// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT Coordinate2D final {
public:
    static_assert(std::is_arithmetic<T>::value,
        "You can only use arithmetic types.");
    typedef T value_type;

    T X, Y;

public:
    // Constructors:
    Coordinate2D() noexcept = default;

    ///@brief Construct from T values.
    Coordinate2D(T x, T y) noexcept;

    // Assignment operators:
    Coordinate2D & operator+=(Coordinate2D const&) noexcept;
    Coordinate2D & operator-=(Coordinate2D const&) noexcept;
    Coordinate2D & operator*=(T scaleFactor) noexcept;
    Coordinate2D & operator/=(T scaleFactor);

    // Unary operators:
    Coordinate2D operator+() const noexcept;
    Coordinate2D operator-() const noexcept;

    // Binary operators:
    Coordinate2D operator+(Coordinate2D const&) const noexcept;
    Coordinate2D operator-(Coordinate2D const&) const noexcept;
    Coordinate2D operator*(Coordinate2D const&) const noexcept;
    Coordinate2D operator/(Coordinate2D const&) const;
    Coordinate2D operator*(T scaleFactor) const noexcept;
    Coordinate2D operator/(T scaleFactor) const;

    bool operator==(Coordinate2D const&) const noexcept;
    bool operator!=(Coordinate2D const&) const noexcept;

    static Coordinate2D const Zero;
};

template <typename T>
Coordinate2D<T> POMDOG_EXPORT
operator*(T scaleFactor, Coordinate2D<T> const& coordinate) noexcept;

template <typename T>
Coordinate2D<T> POMDOG_EXPORT
operator/(T scaleFactor, Coordinate2D<T> const& coordinate);

} // namespace Detail
} // namespace Pomdog
