// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Math/detail/FloatingPointVector3.hpp"
#include "Pomdog/Math/detail/FloatingPointMatrix4x4.hpp"
#include "Pomdog/Math/detail/FloatingPointVector2.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {

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
	: X(x), Y(y), Z(z)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>::FloatingPointVector3(FloatingPointVector2<T> const& vec, T z)
	: X(vec.X), Y(vec.Y), Z(z)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator+=(FloatingPointVector3 const& vec)
{
	X += vec.X;
	Y += vec.Y;
	Z += vec.Z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator-=(FloatingPointVector3 const& vec)
{
	X -= vec.X;
	Y -= vec.Y;
	Z -= vec.Z;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator*=(T scaleFactor)
{
	X *= scaleFactor;
	Y *= scaleFactor;
	Z *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> & FloatingPointVector3<T>::operator/=(T scaleFactor)
{
	X /= scaleFactor;
	Y /= scaleFactor;
	Z /= scaleFactor;
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
	return FloatingPointVector3(-X, -Y, -Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator+(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		X + other.X,
		Y + other.Y,
		Z + other.Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator-(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		X - other.X,
		Y - other.Y,
		Z - other.Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(FloatingPointVector3 const& other) const
{
	return FloatingPointVector3(
		X * other.X,
		Y * other.Y,
		Z * other.Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(FloatingPointVector3 const& other) const
{
	POMDOG_ASSERT(other.X != 0);
	POMDOG_ASSERT(other.Y != 0);
	POMDOG_ASSERT(other.Z != 0);

	return FloatingPointVector3(
		X / other.X,
		Y / other.Y,
		Z / other.Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator*(T scaleFactor) const
{
	return FloatingPointVector3(
		X * scaleFactor,
		Y * scaleFactor,
		Z * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> FloatingPointVector3<T>::operator/(T scaleFactor) const
{
	POMDOG_ASSERT(scaleFactor != 0);
	//static_assert(!std::is_same<decltype(scaleFactor), ChuckNorris>::value,
	//  "NOTE: Chuck Norris can divide by zero.");
	return FloatingPointVector3(
		X / scaleFactor,
		Y / scaleFactor,
		Z / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector3<T>::operator==(FloatingPointVector3 const& other) const
{
	return X == other.X && Y == other.Y && Z == other.Z;
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointVector3<T>::operator!=(FloatingPointVector3 const& other) const
{
	return X != other.X || Y != other.Y || Z != other.Z;
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::Length() const
{
	return std::sqrt(X * X + Y * Y + Z * Z);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointVector3<T>::LengthSquared() const
{
	return X * X + Y * Y + Z * Z;
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
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Cross(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		a.Y * b.Z - a.Z * b.Y,
		a.Z * b.X - a.X * b.Z,
		a.X * b.Y - a.Y * b.X);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Min(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		std::min(a.X, b.X),
		std::min(a.Y, b.Y),
		std::min(a.Z, b.Z));
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Max(FloatingPointVector3 const& a, FloatingPointVector3 const& b)
{
	return FloatingPointVector3(
		std::max(a.X, b.X),
		std::max(a.Y, b.Y),
		std::max(a.Z, b.Z));
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Clamp(FloatingPointVector3 const& source, FloatingPointVector3 const& min, FloatingPointVector3 const& max)
{
	return FloatingPointVector3(
		MathHelper::Clamp(source.X, min.X, max.X),
		MathHelper::Clamp(source.Y, min.Y, max.Y),
		MathHelper::Clamp(source.Z, min.Z, max.Z));
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Lerp(FloatingPointVector3 const& source1, FloatingPointVector3 const& source2, T amount)
{
	return FloatingPointVector3(
		MathHelper::Lerp(source1.X, source2.X, amount),
		MathHelper::Lerp(source1.Y, source2.Y, amount),
		MathHelper::Lerp(source1.Z, source2.Z, amount));
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::SmoothStep(FloatingPointVector3 const& source1, FloatingPointVector3 const& source2, T amount)
{
	return FloatingPointVector3(
		MathHelper::SmoothStep(source1.X, source2.X, amount),
		MathHelper::SmoothStep(source1.Y, source2.Y, amount),
		MathHelper::SmoothStep(source1.Z, source2.Z, amount));
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
	return std::move(result);
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
		result.X = source.X * inverseLength;
		result.Y = source.Y * inverseLength;
		result.Z = source.Z * inverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::Transform(FloatingPointVector3 const& position, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector3(
		(position.X * matrix.m[0][0]) + (position.Y * matrix.m[1][0]) + (position.Z * matrix.m[2][0]) + matrix.m[3][0],
		(position.X * matrix.m[0][1]) + (position.Y * matrix.m[1][1]) + (position.Z * matrix.m[2][1]) + matrix.m[3][1],
		(position.X * matrix.m[0][2]) + (position.Y * matrix.m[1][2]) + (position.Z * matrix.m[2][2]) + matrix.m[3][2]);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T>
FloatingPointVector3<T>::TransformNormal(FloatingPointVector3 const& normal, FloatingPointMatrix4x4<T> const& matrix)
{
	return FloatingPointVector3(
		(normal.X * matrix.m[0][0]) + (normal.Y * matrix.m[1][0]) + (normal.Z * matrix.m[2][0]),
		(normal.X * matrix.m[0][1]) + (normal.Y * matrix.m[1][1]) + (normal.Z * matrix.m[2][1]),
		(normal.X * matrix.m[0][2]) + (normal.Y * matrix.m[1][2]) + (normal.Z * matrix.m[2][2]));
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointVector3<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointVector3<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointVector3<T> operator*(T scaleFactor, FloatingPointVector3<T> const& vector)
{
	return FloatingPointVector3<T>(
		scaleFactor * vector.X,
		scaleFactor * vector.Y,
		scaleFactor * vector.Z);
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

}// namespace Detail
}// namespace Pomdog
