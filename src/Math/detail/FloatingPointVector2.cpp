//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <pomdog/Math/detail/FloatingPointVector2.hpp>
#include <pomdog/Utility/Assert.hpp>
#include <cmath>
#include <limits>
#include <pomdog/Math/detail/FloatingPointMatrix4x4.hpp>
#include <pomdog/Math/detail/FloatingPointQuaternion.hpp>

namespace Pomdog {
namespace Details {

//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>::FloatingPointVector2(T x, T y)
	: x(x), y(y)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator+=(FloatingPointVector2 const& other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator-=(FloatingPointVector2 const& other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator*=(T scaleFactor)
{
	x *= scaleFactor;
	y *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>& FloatingPointVector2<T>::operator/=(T scaleFactor)
{
	this->x /= scaleFactor;
	this->y /= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator+() const
{
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator-() const
{
	return FloatingPointVector2(-this->x, -this->y);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator*(T scaleFactor) const
{
	return FloatingPointVector2(this->x * scaleFactor, this->y * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> 
FloatingPointVector2<T> FloatingPointVector2<T>::operator/(T scaleFactor) const
{
	return FloatingPointVector2(this->x / scaleFactor, this->y / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T> 
FloatingPointVector2<T> operator*(T scaleFactor, FloatingPointVector2<T> const& other)
{
	return FloatingPointVector2<T>(scaleFactor * other.x, scaleFactor * other.y);
}
//-----------------------------------------------------------------------
template <typename T> 
FloatingPointVector2<T> operator/(T scaleFactor, FloatingPointVector2<T> const& other)
{
	return FloatingPointVector2<T>(scaleFactor / other.x, scaleFactor / other.y);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator+(FloatingPointVector2 const& other) const
{
	return FloatingPointVector2(this->x + other.x, this->y + other.y);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator-(FloatingPointVector2 const& other) const
{
	return FloatingPointVector2(this->x - other.x, this->y - other.y);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator*(FloatingPointVector2 const& other) const
{
	return FloatingPointVector2(this->x * other.x, this->y * other.y);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T> FloatingPointVector2<T>::operator/(FloatingPointVector2 const& other) const
{
	return FloatingPointVector2(this->x / other.x, this->y / other.y);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector2<T>::operator==(FloatingPointVector2 const& other) const
{
	return this->x == other.x && this->y == other.y;
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector2<T>::operator!=(FloatingPointVector2 const& other) const
{
	return this->x != other.x || this->y != other.y;
}
//-----------------------------------------------------------------------
template <typename T>
T
FloatingPointVector2<T>::Length() const
{
	return std::sqrt(x * x + y * y);
}
//-----------------------------------------------------------------------
template <typename T>
T
FloatingPointVector2<T>::LengthSquared() const
{
	return x * x + y * y;
}
//-----------------------------------------------------------------------
template <typename T>
T
FloatingPointVector2<T>::Distance(FloatingPointVector2 const& a, FloatingPointVector2 const& b)
{
	return (a - b).Length();
}
//-----------------------------------------------------------------------
template <typename T>
T
FloatingPointVector2<T>::Dot(FloatingPointVector2 const& a, FloatingPointVector2 const& b)
{
	return a.x * b.x + a.y * b.y;
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointVector2<T>::Normalize()
{
	auto const length = Length();

	if (length > std::numeric_limits<decltype(length)>::epsilon())
	{
		POMDOG_ASSERT(length != static_cast<T>(0));
		auto const InverseLength = static_cast<T>(1) / length;
		x *= InverseLength;
		y *= InverseLength;
		return;
	}
	else {
		x = static_cast<T>(0);
		y = static_cast<T>(0);
	}
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Normalize(FloatingPointVector2 const& source)
{
	FloatingPointVector2 result;
	Normalize(source, result);
	return result;
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointVector2<T>::Normalize(FloatingPointVector2 const& source, FloatingPointVector2 & result)
{
	auto const length = source.Length();

	if (length > std::numeric_limits<decltype(length)>::epsilon())
	{
		POMDOG_ASSERT(length != static_cast<T>(0));
		auto const InverseLength = static_cast<T>(1) / length;
		result.x = source.x * InverseLength;
		result.y = source.y * InverseLength;
		return;
	}
	result.x = static_cast<T>(0);
	result.y = static_cast<T>(0);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Transform(FloatingPointVector2 const& position, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector2(
		(position.x * matrix.m[0][0]) + (position.y * matrix.m[1][0]) + matrix.m[3][0], 
		(position.x * matrix.m[0][1]) + (position.y * matrix.m[1][1]) + matrix.m[3][1]
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::Transform(FloatingPointVector2 const& position, FloatingPointQuaternion<T> const& rotation)
{
	auto const x = 2 * (position.y * -rotation.z);
	auto const y = 2 * (position.x * rotation.z);
	auto const z = 2 * (position.y * rotation.x - position.x * rotation.y);

	return FloatingPointVector2(
		position.x + x * rotation.w + (rotation.y * z - rotation.z * y),
		position.y + y * rotation.w + (rotation.z * x - rotation.x * z)
	);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector2<T>
FloatingPointVector2<T>::TransformNormal(FloatingPointVector2 const& normal, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector2(
		(normal.x * matrix.m[0][0]) + (normal.y * matrix.m[1][0]), 
		(normal.x * matrix.m[0][1]) + (normal.y * matrix.m[1][1])
	);
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointVector2<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointVector2<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &x;
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointVector2<float>;

#if defined(DBL_MANT_DIG)
template class FloatingPointVector2<double>;
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointVector2<long double>;
#endif

}// namespace Details
}// namespace Pomdog
