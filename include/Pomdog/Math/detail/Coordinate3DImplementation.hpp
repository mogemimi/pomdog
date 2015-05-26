// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COORDINATE3DIMPLEMENTATION_4B7428C8_HPP
#define POMDOG_COORDINATE3DIMPLEMENTATION_4B7428C8_HPP

//#include "../Coordinate3D.hpp"

namespace Pomdog {
namespace Detail {

template <typename T>
Coordinate3D<T> const Coordinate3D<T>::Zero(0, 0, 0);
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T>::Coordinate3D(T x, T y, T z) noexcept
    : X(x), Y(y), Z(z)
{}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator+=(Coordinate3D const& other)
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator-=(Coordinate3D const& other)
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator*=(T scaleFactor)
{
    X *= scaleFactor;
    Y *= scaleFactor;
    Z *= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator/=(T scaleFactor)
{
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator+() const
{
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator-() const
{
    return Coordinate3D(-X, -Y, -Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator+(Coordinate3D const& other) const
{
    return Coordinate3D(X + other.X, Y + other.Y, Z + other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator-(Coordinate3D const& other) const
{
    return Coordinate3D(X - other.X, Y - other.Y, Z - other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(Coordinate3D const& other) const
{
    return Coordinate3D(X * other.X, Y * other.Y, Z * other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(Coordinate3D const& other) const
{
    return Coordinate3D(X / other.X, Y / other.Y, Z / other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(T scaleFactor) const
{
    return Coordinate3D(X * scaleFactor, Y * scaleFactor, Z * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(T scaleFactor) const
{
    return Coordinate3D(X / scaleFactor, Y / scaleFactor, Z / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator==(Coordinate3D const& other) const
{
    return (X == other.X && Y == other.Y && Z == other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator!=(Coordinate3D const& other) const
{
    return (X != other.X || Y != other.Y || Z != other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator*(T scaleFactor, Coordinate3D<T> const& coordinate)
{
    return Coordinate3D<T>(
        scaleFactor * coordinate.X,
        scaleFactor * coordinate.Y,
        scaleFactor * coordinate.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator/(T scaleFactor, Coordinate3D<T> const& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    POMDOG_ASSERT(coordinate.Z != 0);

    return Coordinate3D<T>(
        scaleFactor / coordinate.X,
        scaleFactor / coordinate.Y,
        scaleFactor / coordinate.Z);
}

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_COORDINATE3DIMPLEMENTATION_4B7428C8_HPP
