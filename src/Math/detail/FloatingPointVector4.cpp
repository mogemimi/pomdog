//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/detail/FloatingPointVector4.hpp>
#include <cmath>
#include <limits>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/detail/FloatingPointVector3.hpp>

namespace Pomdog {
namespace Details {

template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(FloatingPointVector3<T> const& xyz, T w)
	: X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(T x, T y, T z, T w)
	: X(x), Y(y), Z(z), W(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator+=(FloatingPointVector4 const& other)
{
	X += other.X;
	Y += other.Y;
	Z += other.Z;
	W += other.W;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator-=(FloatingPointVector4 const& other)
{
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
	W -= other.W;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator*=(T scaleFactor)
{
	X *= scaleFactor;
	Y *= scaleFactor;
	Z *= scaleFactor;
	W *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator/=(T scaleFactor)
{
	X /= scaleFactor;
	Y /= scaleFactor;
	Z /= scaleFactor;
	W /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator-() const
{
	return FloatingPointVector4(-X, -Y, -Z, -W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator+(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		X + other.X,
		Y + other.Y,
		Z + other.Z,
		W + other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator-(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		X - other.X,
		Y - other.Y,
		Z - other.Z,
		W - other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		X * other.X,
		Y * other.Y,
		Z * other.Z,
		W * other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		X / other.X,
		Y / other.Y,
		Z / other.Z,
		W / other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(T scaleFactor) const
{
	return FloatingPointVector4(
		X * scaleFactor,
		Y * scaleFactor,
		Z * scaleFactor,
		W * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(T scaleFactor) const
{
	POMDOG_ASSERT(scaleFactor != 0);
	//static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
	//  "NOTE: Chuck Norris can divide by zero.");
	return FloatingPointVector4(
		X / scaleFactor,
		Y / scaleFactor,
		Z / scaleFactor,
		W / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector4<T>::operator==(FloatingPointVector4 const& other) const
{
	return (X == other.X && Y == other.Y && Z == other.Z && W == other.W);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector4<T>::operator!=(FloatingPointVector4 const& other) const
{
	return (X != other.X || Y != other.Y || Z != other.Z || W != other.W);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::Length() const
{
	return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::LengthSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::Distance(FloatingPointVector4 const& a, FloatingPointVector4 const& b)
{
	return (a - b).Length();
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::Dot(FloatingPointVector4 const& a, FloatingPointVector4 const& b)
{
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}
//-----------------------------------------------------------------------
template <typename T>
void FloatingPointVector4<T>::Normalize()
{
	*this = Normalize(*this);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T>
FloatingPointVector4<T>::Normalize(FloatingPointVector4 const& source)
{
	FloatingPointVector4 result;
	Normalize(source, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
void FloatingPointVector4<T>::Normalize(FloatingPointVector4 const& source, FloatingPointVector4 & result)
{
	auto const length = source.Length();

	if (length > std::numeric_limits<decltype(length)>::epsilon())
	{
		constexpr T One = 1;
		auto const inverseLength = One / length;
		result.X = source.X * inverseLength;
		result.Y = source.Y * inverseLength;
		result.Z = source.Z * inverseLength;
		result.W = source.W * inverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointVector4<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointVector4<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> operator*(T scaleFactor, FloatingPointVector4<T> const& vector)
{
	return FloatingPointVector4<T>(
		scaleFactor * vector.X,
		scaleFactor * vector.Y,
		scaleFactor * vector.Z,
		scaleFactor * vector.W);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointVector4<float>;
template FloatingPointVector4<float> operator*<float>(float, FloatingPointVector4<float> const&);

#if defined(DBL_MANT_DIG)
template class FloatingPointVector4<double>;
template FloatingPointVector4<double> operator*<double>(double, FloatingPointVector4<double> const&);
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector4<long double>;
template FloatingPointVector4<long double> operator*<long double>(long double, FloatingPointVector4<long double> const&);
#endif

}// namespace Details
}// namespace Pomdog
