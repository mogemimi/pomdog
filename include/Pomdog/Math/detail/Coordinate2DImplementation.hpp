// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP
#define POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP

//#include "../Coordinate2D.hpp"

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
Coordinate2D<T> & Coordinate2D<T>::operator+=(Coordinate2D const& other)
{
    X += other.X;
    Y += other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator-=(Coordinate2D const& other)
{
    X -= other.X;
    Y -= other.Y;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator*=(T scaleFactor)
{
    X *= scaleFactor;
    Y *= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator/=(T scaleFactor)
{
    X /= scaleFactor;
    Y /= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator+() const
{
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator-() const
{
    return Coordinate2D(-X, -Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator+(Coordinate2D const& other) const
{
    return Coordinate2D(X + other.X, Y + other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator-(Coordinate2D const& other) const
{
    return Coordinate2D(X - other.X, Y - other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(Coordinate2D const& other) const
{
    return Coordinate2D(X * other.X, Y * other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(Coordinate2D const& other) const
{
    return Coordinate2D(X / other.X, Y / other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(T scaleFactor) const
{
    return Coordinate2D(X * scaleFactor, Y * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(T scaleFactor) const
{
    return Coordinate2D(X / scaleFactor, Y / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator==(Coordinate2D const& other) const
{
    return (X == other.X && Y == other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator!=(Coordinate2D const& other) const
{
    return (X != other.X || Y != other.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator*(T scaleFactor, Coordinate2D<T> const& coordinate)
{
    return Coordinate2D<T>(scaleFactor * coordinate.X, scaleFactor * coordinate.Y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator/(T scaleFactor, Coordinate2D<T> const& coordinate)
{
    return Coordinate2D<T>(scaleFactor / coordinate.X, scaleFactor / coordinate.Y);
}

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_COORDINATE2DIMPLEMENTATION_3BBB93C4_HPP
