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
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/detail/FloatingPointVector3.hpp>

namespace Pomdog {
namespace Details {

//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(FloatingPointVector3<T> const& xyz, T w)
	: x(xyz.x), y(xyz.y), z(xyz.z), w(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T>::FloatingPointVector4(T x, T y, T z, T w)
	: x(x), y(y), z(z), w(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator+=(FloatingPointVector4 const& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator-=(FloatingPointVector4 const& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	z *= scaleFactor;
	w *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> & FloatingPointVector4<T>::operator/=(T scaleFactor)
{
	x /= scaleFactor;
	y /= scaleFactor;
	z /= scaleFactor;
	w /= scaleFactor;
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
	return FloatingPointVector4(-x, -y, -z, -w);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator+(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		x + other.x,
		y + other.y,
		z + other.z,
		w + other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator-(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		x - other.x,
		y - other.y,
		z - other.z,
		w - other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		x * other.x,
		y * other.y,
		z * other.z,
		w * other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(FloatingPointVector4 const& other) const
{
	return FloatingPointVector4(
		x / other.x,
		y / other.y,
		z / other.z,
		w / other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator*(T scaleFactor) const
{
	return FloatingPointVector4(
		x * scaleFactor,
		y * scaleFactor,
		z * scaleFactor,
		w * scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> FloatingPointVector4<T>::operator/(T scaleFactor) const
{
	POMDOG_ASSERT(scaleFactor != 0);
	//static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
	//  "Notes: Chuck Norris can divide by zero.");
	return FloatingPointVector4(
		x / scaleFactor,
		y / scaleFactor,
		z / scaleFactor,
		w / scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector4<T>::operator==(FloatingPointVector4 const& other) const
{
	return (x == other.x && y == other.y && z == other.z && w == other.w);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector4<T>::operator!=(FloatingPointVector4 const& other) const
{
	return (x != other.x || y != other.y || z != other.z || w != other.w);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::Length() const
{
	return std::sqrt(x * x + y * y + z * z + w * w);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector4<T>::LengthSquared() const
{
	return x * x + y * y + z * z + w * w;
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
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
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
	return result;
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
		result.x = source.x * inverseLength;
		result.y = source.y * inverseLength;
		result.z = source.z * inverseLength;
		result.w = source.w * inverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointVector4<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointVector4<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector4<T> operator*(T scaleFactor, FloatingPointVector4<T> const& vector)
{
	return FloatingPointVector4<T>(
		scaleFactor * vector.x,
		scaleFactor * vector.y,
		scaleFactor * vector.z,
		scaleFactor * vector.w
	);
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
