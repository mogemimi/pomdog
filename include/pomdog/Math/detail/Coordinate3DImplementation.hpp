//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_COORDINATE3DIMPLEMENTATION_HPP
#define POMDOG_DETAIL_COORDINATE3DIMPLEMENTATION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

//#include "../Coordinate3D.hpp"

namespace Pomdog {

//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T>::Coordinate3D(T x, T y, T z)
	: x(x), y(y), z(z)
{}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator+=(Coordinate3D const& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator-=(Coordinate3D const& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	z *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> & Coordinate3D<T>::operator/=(T scaleFactor)
{
	x /= scaleFactor;
	y /= scaleFactor;
	z /= scaleFactor;
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
	return Coordinate3D(-x, -y, -z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator+(Coordinate3D const& other) const
{
	return Coordinate3D(x + other.x, y + other.y, z + other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator-(Coordinate3D const& other) const
{
	return Coordinate3D(x - other.x, y - other.y, z - other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(Coordinate3D const& other) const
{
	return Coordinate3D(x * other.x, y * other.y, z * other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(Coordinate3D const& other) const
{
	return Coordinate3D(x / other.x, y / other.y, z / other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator*(T scaleFactor) const
{
	return Coordinate3D(x * scaleFactor, y * scaleFactor, z * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> Coordinate3D<T>::operator/(T scaleFactor) const
{
	return Coordinate3D(x / scaleFactor, y / scaleFactor, z / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator==(Coordinate3D const& other) const
{
	return (x == other.x && y == other.y && z == other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate3D<T>::operator!=(Coordinate3D const& other) const
{
	return (x != other.x || y != other.y || z != other.z);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator*(T scaleFactor, Coordinate3D<T> const& coordinate)
{
	return Coordinate3D<T>(
		scaleFactor * coordinate.x,
		scaleFactor * coordinate.y,
		scaleFactor * coordinate.z
	);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate3D<T> operator/(T scaleFactor, Coordinate3D<T> const& coordinate)
{
	return Coordinate3D<T>(
		scaleFactor / coordinate.x,
		scaleFactor / coordinate.y,
		scaleFactor / coordinate.z
	);
}

}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_COORDINATE3DIMPLEMENTATION_HPP)
