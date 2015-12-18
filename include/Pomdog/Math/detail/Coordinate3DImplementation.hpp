// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

//#include "../Coordinate3D.hpp"
#include "Pomdog/Utility/Assert.hpp"

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
Coordinate3D<T> & Coordinate3D<T>::operator+=(Coordinate3D const& other) noexcept
{
    X += other.X;
    Y += other.Y;
    Z += other.Z;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator-=(Coordinate3D const& other) noexcept
{
    X -= other.X;
    Y -= other.Y;
    Z -= other.Z;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator*=(T scaleFactor) noexcept
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
    POMDOG_ASSERT(scaleFactor != 0);
    X /= scaleFactor;
    Y /= scaleFactor;
    Z /= scaleFactor;
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator+() const noexcept
{
    return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator-() const noexcept
{
    return {-X, -Y, -Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator+(Coordinate3D const& other) const noexcept
{
    return {X + other.X, Y + other.Y, Z + other.Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator-(Coordinate3D const& other) const noexcept
{
    return {X - other.X, Y - other.Y, Z - other.Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(Coordinate3D const& other) const noexcept
{
    return {X * other.X, Y * other.Y, Z * other.Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(Coordinate3D const& other) const
{
    POMDOG_ASSERT(other.X != 0);
    POMDOG_ASSERT(other.Y != 0);
    POMDOG_ASSERT(other.Z != 0);
    return {X / other.X, Y / other.Y, Z / other.Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(T scaleFactor) const noexcept
{
    return {X * scaleFactor, Y * scaleFactor, Z * scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(T scaleFactor) const
{
    POMDOG_ASSERT(scaleFactor != 0);
    return {X / scaleFactor, Y / scaleFactor, Z / scaleFactor};
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator==(Coordinate3D const& other) const noexcept
{
    return (X == other.X && Y == other.Y && Z == other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator!=(Coordinate3D const& other) const noexcept
{
    return (X != other.X || Y != other.Y || Z != other.Z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator*(T scaleFactor, Coordinate3D<T> const& coordinate) noexcept
{
    return Coordinate3D<T>{
        scaleFactor * coordinate.X,
        scaleFactor * coordinate.Y,
        scaleFactor * coordinate.Z};
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator/(T scaleFactor, Coordinate3D<T> const& coordinate)
{
    POMDOG_ASSERT(coordinate.X != 0);
    POMDOG_ASSERT(coordinate.Y != 0);
    POMDOG_ASSERT(coordinate.Z != 0);

    return Coordinate3D<T>{
        scaleFactor / coordinate.X,
        scaleFactor / coordinate.Y,
        scaleFactor / coordinate.Z};
}

} // namespace Detail
} // namespace Pomdog
