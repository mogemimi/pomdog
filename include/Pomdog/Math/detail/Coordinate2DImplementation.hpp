//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MATH_DETAIL_COORDINATE2DIMPLEMENTATION_3BBB93C4_83C5_4AD1_AFEE_F79A0DCF06EC_HPP
#define POMDOG_MATH_DETAIL_COORDINATE2DIMPLEMENTATION_3BBB93C4_83C5_4AD1_AFEE_F79A0DCF06EC_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

//#include "../Coordinate2D.hpp"

namespace Pomdog {
namespace Details {

template <typename T>
Coordinate2D<T> const Coordinate2D<T>::Zero(0, 0);
//-----------------------------------------------------------------------
template <typename T> inline
Coordinate2D<T>::Coordinate2D(T x, T y)
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

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_COORDINATE2DIMPLEMENTATION_3BBB93C4_83C5_4AD1_AFEE_F79A0DCF06EC_HPP)
