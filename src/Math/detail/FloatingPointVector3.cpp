//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/detail/FloatingPointVector3.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <cmath>
#include <limits>
#include <Pomdog/Math/detail/FloatingPointMatrix4x4.hpp>
#include <Pomdog/Math/detail/FloatingPointVector2.hpp>
#include <Pomdog/Math/MathHelper.hpp>

namespace Pomdog {
namespace Details {

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::Zero(0, 0, 0);

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitX(1, 0, 0);

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitY(0, 1, 0);

template <typename T>
FloatingPointVector3<T> const FloatingPointVector3<T>::UnitZ(0, 0, 1);
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>::FloatingPointVector3(T x, T y, T z)
	: x(x), y(y), z(z)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>::FloatingPointVector3(FloatingPointVector2<T> const& vec, T z)
	: x(vec.x), y(vec.y), z(z)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator+=(FloatingPointVector3 const& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator-=(FloatingPointVector3 const& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	z *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator/=(T scaleFactor)
{
	x /= scaleFactor;
	y /= scaleFactor;
	z /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator-() const
{
	return FloatingPointVector3(-x, -y, -z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator+(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		x + other.x,
		y + other.y,
		z + other.z
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator-(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		x - other.x,
		y - other.y,
		z - other.z
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		x * other.x,
		y * other.y,
		z * other.z
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		x / other.x,
		y / other.y,
		z / other.z
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(T scaleFactor) const
{
	return FloatingPointVector3(
		x * scaleFactor,
		y * scaleFactor,
		z * scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(T scaleFactor) const
{
	return FloatingPointVector3(
		x / scaleFactor,
		y / scaleFactor,
		z / scaleFactor
	);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector3<T>::operator==(FloatingPointVector3 const& other) const
{
	return (x == other.x && y == other.y && z == other.z);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector3<T>::operator!=(FloatingPointVector3 const& other) const
{
	return (x != other.x || y != other.y || z != other.z);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::Length() const
{
	return std::sqrt(x * x + y * y + z * z);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::LengthSquared() const
{
	return x * x + y * y + z * z;
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::Distance(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return (a - b).Length();
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::DistanceSquared(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return (a - b).LengthSquared();
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::Dot(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Cross(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Min(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		std::min(a.x, b.x),
		std::min(a.y, b.y),
		std::min(a.z, b.z)
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Max(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		std::max(a.x, b.x),
		std::max(a.y, b.y),
		std::max(a.z, b.z)
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Clamp(FloatingPointVector3 const& source, FloatingPointVector3 const& min, FloatingPointVector3 const& max)
{
	return FloatingPointVector3(
		MathHelper::Clamp(source.x, min.x, max.x),
		MathHelper::Clamp(source.y, min.y, max.y),
		MathHelper::Clamp(source.z, min.z, max.z)
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::SmoothStep(FloatingPointVector3 const& a, FloatingPointVector3 const& b, T amount)
{
	return FloatingPointVector3(
		MathHelper::SmoothStep(a.x, b.x, amount),
		MathHelper::SmoothStep(a.y, b.y, amount),
		MathHelper::SmoothStep(a.z, b.z, amount)
	);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointVector3<T>::Normalize()
{
	*this = Normalize(*this);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Normalize(FloatingPointVector3 const& source)
{
	FloatingPointVector3 result;
	Normalize(source, result);
	return result;
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointVector3<T>::Normalize(FloatingPointVector3 const& source, FloatingPointVector3 & result)
{
	auto const length = source.Length();

	if (length > std::numeric_limits<decltype(length)>::epsilon())
	{
		constexpr T One = 1;
		auto const inverseLength = One / length;
		result.x = source.x * inverseLength;
		result.y = source.y * inverseLength;
		result.z = source.z * inverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Transform(FloatingPointVector3 const& position, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector3(
		(position.x * matrix.m[0][0]) + (position.y * matrix.m[1][0]) + (position.z * matrix.m[2][0]) + matrix.m[3][0],
		(position.x * matrix.m[0][1]) + (position.y * matrix.m[1][1]) + (position.z * matrix.m[2][1]) + matrix.m[3][1],
		(position.x * matrix.m[0][2]) + (position.y * matrix.m[1][2]) + (position.z * matrix.m[2][2]) + matrix.m[3][2]
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::TransformNormal(FloatingPointVector3 const& normal, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector3(
		(normal.x * matrix.m[0][0]) + (normal.y * matrix.m[1][0]) + (normal.z * matrix.m[2][0]),
		(normal.x * matrix.m[0][1]) + (normal.y * matrix.m[1][1]) + (normal.z * matrix.m[2][1]),
		(normal.x * matrix.m[0][2]) + (normal.y * matrix.m[1][2]) + (normal.z * matrix.m[2][2])
	);
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointVector3<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointVector3<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> operator*(T scaleFactor, FloatingPointVector3<T> const& vector)
{
	return FloatingPointVector3<T>(
		scaleFactor * vector.x,
		scaleFactor * vector.y,
		scaleFactor * vector.z
	);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointVector3<float>;
template FloatingPointVector3<float> operator*<float>(float, FloatingPointVector3<float> const&);

#if defined(DBL_MANT_DIG)
template class FloatingPointVector3<double>;
template FloatingPointVector3<double> operator*<double>(double, FloatingPointVector3<double> const&);
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector3<long double>;
template FloatingPointVector3<long double> operator*<long double>(long double, FloatingPointVector3<long double> const&);
#endif

}// namespace Details
}// namespace Pomdog
