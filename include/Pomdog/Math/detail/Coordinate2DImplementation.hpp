// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP
#define POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP

//#include "../Coordinate2D.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {

template <typename T>
Coordinate2D<T> const Coordinate2D<T>::Zero(0, 0);
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T>::Coordinate2D(T x, T y) noexcept
    : X(x), Y(y)
{}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator+=(Coordinate2D const& other) noexcept
{
    X += other.X;
    Y += other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator-=(Coordinate2D const& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator*=(T scaleFactor) noexcept
{
    X *= scaleFactor;
    Y *= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator/=(T scaleFactor)
{
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator+() const noexcept
{
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator-() const noexcept
{
    return {-X, -Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator+(Coordinate2D const& other) const noexcept
{
    return {X + other.X, Y + other.Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator-(Coordinate2D const& other) const noexcept
{
    return {X - other.X, Y - other.Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(Coordinate2D const& other) const noexcept
{
    return {X * other.X, Y * other.Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(Coordinate2D const& other) const
{
    return {X / other.X, Y / other.Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(T scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator==(Coordinate2D const& other) const noexcept
{
    return (X == other.X && Y == other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator!=(Coordinate2D const& other) const noexcept
{
    return (X != other.X || Y != other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator*(T scaleFactor, Coordinate2D<T> const& coordinate)
{
    return {scaleFactor * coordinate.X, scaleFactor * coordinate.Y};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator/(T scaleFactor, Coordinate2D<T> const& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    return {scaleFactor / coordinate.X, scaleFactor / coordinate.Y};
}

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP
