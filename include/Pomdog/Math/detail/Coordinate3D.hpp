// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COORDINATE3D_6062BA6C_HPP
#define POMDOG_COORDINATE3D_6062BA6C_HPP

#include "Pomdog/Basic/Export.hpp"
#include <type_traits>

namespace Pomdog {
namespace Detail {

template <typename T>
class POMDOG_EXPORT Coordinate3D final {
public:
    static_assert(std::is_arithmetic<T>::value, "You can only use arithmetic types.");
    typedef T value_type;

    T X, Y, Z;

public:
    // Constructors:
    Coordinate3D() noexcept = default;

    ///@brief Construct from T values.
    Coordinate3D(T x, T y, T z) noexcept;

    // Assignment operators:
    Coordinate3D & operator+=(Coordinate3D const&);
    Coordinate3D & operator-=(Coordinate3D const&);
    Coordinate3D & operator*=(T scaleFactor);
    Coordinate3D & operator/=(T scaleFactor);

    // Unary operators:
    Coordinate3D operator+() const;
    Coordinate3D operator-() const;

    // Binary operators:
    Coordinate3D operator+(Coordinate3D const&) const;
    Coordinate3D operator-(Coordinate3D const&) const;
    Coordinate3D operator*(Coordinate3D const&) const;
    Coordinate3D operator/(Coordinate3D const&) const;
    Coordinate3D operator*(T scaleFactor) const;
    Coordinate3D operator/(T scaleFactor) const;

    bool operator==(Coordinate3D const&) const;
    bool operator!=(Coordinate3D const&) const;

    static Coordinate3D const Zero;
};

template <typename T>
Coordinate3D<T> POMDOG_EXPORT
operator*(T scaleFactor, Coordinate3D<T> const& coordinate);

} // namespace Detail
} // namespace Pomdog

#include "Coordinate3DImplementation.hpp"

#endif // POMDOG_COORDINATE3D_6062BA6C_HPP
