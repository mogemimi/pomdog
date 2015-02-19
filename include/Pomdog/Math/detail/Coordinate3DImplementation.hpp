//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MATH_DETAIL_COORDINATE3DIMPLEMENTATION_4B7428C8_1C28_4604_949D_0B5D766C171C_HPP
#define POMDOG_MATH_DETAIL_COORDINATE3DIMPLEMENTATION_4B7428C8_1C28_4604_949D_0B5D766C171C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

//#include "../Coordinate3D.hpp"

namespace Pomdog {
namespace Details {

template <typename T>
Coordinate3D<T> const Coordinate3D<T>::Zero(0, 0, 0);
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T>::Coordinate3D(T x, T y, T z)
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

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_COORDINATE3DIMPLEMENTATION_4B7428C8_1C28_4604_949D_0B5D766C171C_HPP)
