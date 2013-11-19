//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_COORDINATE2DIMPLEMENTATION_HPP
#define POMDOG_DETAIL_COORDINATE2DIMPLEMENTATION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

//#include "../Coordinate2D.hpp"

namespace Pomdog {
namespace Details {

//-----------------------------------------------------------------------
template <typename T>
Coordinate2D<T> const Coordinate2D<T>::Zero(0, 0);
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T>::Coordinate2D(T x, T y)
	: x(x), y(y)
{}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator+=(Coordinate2D const& other)
{
	x += other.x;
	y += other.y;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator-=(Coordinate2D const& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> & Coordinate2D<T>::operator/=(T scaleFactor)
{
	x /= scaleFactor;
	y /= scaleFactor;
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
	return Coordinate2D(-x, -y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator+(Coordinate2D const& other) const
{
	return Coordinate2D(x + other.x, y + other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator-(Coordinate2D const& other) const
{
	return Coordinate2D(x - other.x, y - other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(Coordinate2D const& other) const
{
	return Coordinate2D(x * other.x, y * other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(Coordinate2D const& other) const
{
	return Coordinate2D(x / other.x, y / other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator*(T scaleFactor) const
{
	return Coordinate2D(x * scaleFactor, y * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> Coordinate2D<T>::operator/(T scaleFactor) const
{
	return Coordinate2D(x / scaleFactor, y / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator==(Coordinate2D const& other) const
{
	return (x == other.x && y == other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
bool Coordinate2D<T>::operator!=(Coordinate2D const& other) const
{
	return (x != other.x || y != other.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator*(T scaleFactor, Coordinate2D<T> const& coordinate)
{
	return Coordinate2D<T>(scaleFactor * coordinate.x, scaleFactor * coordinate.y);
}
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T> operator/(T scaleFactor, Coordinate2D<T> const& coordinate)
{
	return Coordinate2D<T>(scaleFactor / coordinate.x, scaleFactor / coordinate.y);
}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_COORDINATE2DIMPLEMENTATION_HPP)
