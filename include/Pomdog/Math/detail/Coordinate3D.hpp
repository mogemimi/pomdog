// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT Coordinate3D final {
public:
    static_assert(std::is_arithmetic<T>::value,
        "You can only use arithmetic types.");
    typedef T value_type;

    T X, Y, Z;

public:
    // Constructors:
    Coordinate3D() noexcept = default;

    ///@brief Construct from T values.
    Coordinate3D(T x, T y, T z) noexcept;

    // Assignment operators:
    Coordinate3D& operator+=(const Coordinate3D&) noexcept;
    Coordinate3D& operator-=(const Coordinate3D&) noexcept;
    Coordinate3D& operator*=(T scaleFactor) noexcept;
    Coordinate3D& operator/=(T scaleFactor);

    // Unary operators:
    Coordinate3D operator+() const noexcept;
    Coordinate3D operator-() const noexcept;

    // Binary operators:
    Coordinate3D operator+(const Coordinate3D&) const noexcept;
    Coordinate3D operator-(const Coordinate3D&) const noexcept;
    Coordinate3D operator*(const Coordinate3D&) const noexcept;
    Coordinate3D operator/(const Coordinate3D&) const;
    Coordinate3D operator*(T scaleFactor) const noexcept;
    Coordinate3D operator/(T scaleFactor) const;

    bool operator==(const Coordinate3D&) const noexcept;
    bool operator!=(const Coordinate3D&) const noexcept;

    static Coordinate3D const Zero;
};

template <typename T>
Coordinate3D<T> const Coordinate3D<T>::Zero = {0, 0, 0};

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator*(T scaleFactor, const Coordinate3D<T>& coordinate) noexcept;

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator/(T scaleFactor, const Coordinate3D<T>& coordinate);

} // namespace Detail
} // namespace Pomdog
