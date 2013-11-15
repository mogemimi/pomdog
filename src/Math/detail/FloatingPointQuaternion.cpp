//
//  Copyright (C) 2013 the Pomdog Engine authors.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Math/detail/FloatingPointQuaternion.hpp>
#include <pomdog/Utility/Assert.hpp>
#include <cmath>
#include <limits>

namespace Pomdog {
namespace Details {

template <typename T>
FloatingPointQuaternion<T> const FloatingPointQuaternion<T>::Zero(0, 0, 0, 0);
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>::FloatingPointQuaternion(T x, T y, T z, T w)
	: x(x), y(y), z(z), w(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator+=(FloatingPointQuaternion const& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator-=(FloatingPointQuaternion const& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator*=(FloatingPointQuaternion const& other)
{
	return (*this) = (*this) * other;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	z *= scaleFactor;
	w *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator/=(T scaleFactor)
{
	x /= scaleFactor;
	y /= scaleFactor;
	z /= scaleFactor;
	w /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator-() const
{
	return FloatingPointQuaternion(-x, -y, -z, -w);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator+(FloatingPointQuaternion const& other) const
{
	return FloatingPointQuaternion(
		x + other.x,
		y + other.y,
		z + other.z,
		w + other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator-(FloatingPointQuaternion const& other) const
{
	return FloatingPointQuaternion(
		x - other.x,
		y - other.y,
		z - other.z,
		w - other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(FloatingPointQuaternion const& other) const
{
	// Quaternion(
	//     w * other.xyz + xyz * other.w + Vector3::Cross(this->xyz, other.xyz),
	//     w * other.w - Vector3::Dot(this->xyz, other.xyz)
	// );
	return FloatingPointQuaternion(
		w * other.x + x * other.w + y * other.z - z * other.y,
		w * other.y + y * other.w + z * other.x - x * other.z,
		w * other.z + z * other.w + x * other.y - y * other.x,
		w * other.w - x * other.x - y * other.y - z * other.z
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(T scaleFactor) const
{
	return FloatingPointQuaternion(
		x * scaleFactor,
		y * scaleFactor,
		z * scaleFactor,
		w * scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator/(T scaleFactor) const
{
	POMDOG_ASSERT(scaleFactor != 0);
	return FloatingPointQuaternion(
		x / scaleFactor,
		y / scaleFactor,
		z / scaleFactor,
		w / scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> operator*(T scaleFactor, FloatingPointQuaternion<T> const& other)
{
	return FloatingPointQuaternion<T>(
		scaleFactor * other.x,
		scaleFactor * other.y,
		scaleFactor * other.z,
		scaleFactor * other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointQuaternion<T>::operator==(FloatingPointQuaternion const& other) const
{
	return (
		x == other.x &&
		y == other.y &&
		z == other.z &&
		w == other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointQuaternion<T>::operator!=(FloatingPointQuaternion const& other) const
{
	return (
		x != other.x ||
		y != other.y ||
		z != other.z ||
		w != other.w
	);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::Length() const
{
	return std::sqrt(x * x + y * y + z * z + w * w);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::LengthSquared() const
{
	return (x * x + y * y + z * z + w * w);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::Dot(FloatingPointQuaternion const& a, FloatingPointQuaternion const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointQuaternion<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointQuaternion<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointQuaternion<float>;

#if defined(DBL_MANT_DIG)
template class FloatingPointQuaternion<double>;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointQuaternion<long double>;
#endif

}// namespace Details
}// namespace Pomdog
