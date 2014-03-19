//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/detail/FloatingPointQuaternion.hpp>
#include <cmath>
#include <limits>
#include <array>
#include <utility>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Radian.hpp>
#include <Pomdog/Math/detail/FloatingPointMatrix3x3.hpp>
#include <Pomdog/Math/detail/FloatingPointMatrix4x4.hpp>
#include <Pomdog/Math/detail/FloatingPointVector3.hpp>

namespace Pomdog {
namespace Details {

template <typename T>
FloatingPointQuaternion<T> const FloatingPointQuaternion<T>::Identity(0, 0, 0, 1);
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>::FloatingPointQuaternion(T x, T y, T z, T w)
	: X(x), Y(y), Z(z), W(w)
{}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator+=(FloatingPointQuaternion const& other)
{
	X += other.X;
	Y += other.Y;
	Z += other.Z;
	W += other.W;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator-=(FloatingPointQuaternion const& other)
{
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
	W -= other.W;
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
	X *= scaleFactor;
	Y *= scaleFactor;
	Z *= scaleFactor;
	W *= scaleFactor;
	return *this;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> & FloatingPointQuaternion<T>::operator/=(T scaleFactor)
{
	POMDOG_ASSERT(scaleFactor != 0);
	X /= scaleFactor;
	W /= scaleFactor;
	Z /= scaleFactor;
	W /= scaleFactor;
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
	return FloatingPointQuaternion(-X, -Y, -Z, -W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator+(FloatingPointQuaternion const& other) const
{
	return FloatingPointQuaternion(
		X + other.X,
		Y + other.Y,
		Z + other.Z,
		W + other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator-(FloatingPointQuaternion const& other) const
{
	return FloatingPointQuaternion(
		X - other.X,
		Y - other.Y,
		Z - other.Z,
		W - other.W);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(FloatingPointQuaternion const& other) const
{
	// Quaternion(
	//     w * other.Xyz + xyz * other.W + Vector3::Cross(this->xyz, other.Xyz),
	//     w * other.W - Vector3::Dot(this->xyz, other.Xyz));
	return FloatingPointQuaternion(
		W * other.X + X * other.W + Y * other.Z - Z * other.Y,
		W * other.Y + Y * other.W + Z * other.X - X * other.Z,
		W * other.Z + Z * other.W + X * other.Y - Y * other.X,
		W * other.W - X * other.X - Y * other.Y - Z * other.Z);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator*(T scaleFactor) const
{
	return FloatingPointQuaternion(
		X * scaleFactor,
		Y * scaleFactor,
		Z * scaleFactor,
		W * scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> FloatingPointQuaternion<T>::operator/(T scaleFactor) const
{
	POMDOG_ASSERT(scaleFactor != 0);
	return FloatingPointQuaternion(
		X / scaleFactor,
		Y / scaleFactor,
		Z / scaleFactor,
		W / scaleFactor);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointQuaternion<T>::operator==(FloatingPointQuaternion const& other) const
{
	return (
		X == other.X &&
		Y == other.Y &&
		Z == other.Z &&
		W == other.W);
}
//-----------------------------------------------------------------------
template <typename T>
bool FloatingPointQuaternion<T>::operator!=(FloatingPointQuaternion const& other) const
{
	return (
		X != other.X ||
		Y != other.Y ||
		Z != other.Z ||
		W != other.W);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::Length() const
{
	return std::sqrt(X * X + Y * Y + Z * Z + W * W);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::LengthSquared() const
{
	return (X * X + Y * Y + Z * Z + W * W);
}
//-----------------------------------------------------------------------
template <typename T>
T FloatingPointQuaternion<T>::Dot(FloatingPointQuaternion const& a, FloatingPointQuaternion const& b)
{
	return a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::Normalize()
{
	*this = Normalize(*this);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::Normalize(FloatingPointQuaternion const& source, FloatingPointQuaternion & result)
{
	auto const length = source.Length();

	if (length > std::numeric_limits<decltype(length)>::epsilon())
	{
		auto const InverseLength = 1 / length;
		result.X = source.X * InverseLength;
		result.Y = source.Y * InverseLength;
		result.Z = source.Z * InverseLength;
		result.W = source.W * InverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Normalize(FloatingPointQuaternion const& source)
{
	FloatingPointQuaternion result;
	Normalize(source, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount,
	FloatingPointQuaternion & result)
{
	auto cosAngle = FloatingPointQuaternion::Dot(begin, end);

	auto const angle(std::acos(cosAngle));
	auto const inverseSinAngle = 1 / std::sin(angle);
	
	auto const coefficient1 = std::sin((1 - amount) * angle) * inverseSinAngle;
	auto const coefficient2 = std::sin(amount * angle) * inverseSinAngle;
	
	result.X = coefficient1 * begin.X + coefficient2 * end.X;
	result.Y = coefficient1 * begin.Y + coefficient2 * end.Y;
	result.Z = coefficient1 * begin.Z + coefficient2 * end.Z;
	result.W = coefficient1 * begin.W + coefficient2 * end.W;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount)
{
	FloatingPointQuaternion result;
	Slerp(begin, end, amount, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::Inverse(FloatingPointQuaternion const& source, FloatingPointQuaternion & result)
{
	auto const lengthSquared = source.LengthSquared();
	if (0 < lengthSquared)
	{
		auto const inverseLengthSquared = 1 / lengthSquared;
		result.X = source.X * -inverseLengthSquared;
		result.Y = source.Y * -inverseLengthSquared;
		result.Z = source.Z * -inverseLengthSquared;
		result.W = source.W *  inverseLengthSquared;
	}
	// Failed to calculate inverse
	//result = Identity;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Inverse(FloatingPointQuaternion const& source)
{
	FloatingPointQuaternion result;
	Inverse(source, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle,
	FloatingPointQuaternion & result)
{
	auto const halfAngle(angle.value/2);
	auto const sinAngle(std::sin(halfAngle));
	auto const cosAngle(std::cos(halfAngle));

	result.X = axis.X * sinAngle;
	result.Y = axis.Y * sinAngle;
	result.Z = axis.Z * sinAngle;
	result.W = cosAngle;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle)
{
	FloatingPointQuaternion result;
	CreateFromAxisAngle(axis, angle, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <class MatrixClass, typename T> static
void CreateFromRotationMatrixImplementation(MatrixClass const& rotation, FloatingPointQuaternion<T> & result)
{
	// Algorithm from the article "Quaternion Calculus and Fast Animation"
	// by Ken Shoemake, SIGGRAPH 1987 Course Notes.

	static_assert(std::is_same<decltype(rotation[0][0]), T const&>::value ||
		std::is_same<decltype(rotation[0][0]), T>::value, "");

	auto const trace = rotation[0][0] + rotation[1][1] + rotation[2][2];
	constexpr T half = static_cast<T>(0.5);

	if (trace > 0)
	{
		auto root = std::sqrt(trace + 1);
		result.W = half * root;
		root = half / root;
		result.X = (rotation[2][1] - rotation[1][2]) * root;
		result.Y = (rotation[0][2] - rotation[2][0]) * root;
		result.Z = (rotation[1][0] - rotation[0][1]) * root;
	}
	else
	{
		std::size_t i = 0;
		if (rotation[1][1] > rotation[0][0])
			i = 1;
		if (rotation[2][2] > rotation[i][i])
			i = 2;

		// (i, j, k) = (0, 1, 2), (1, 2, 0) or (2, 0, 1).
		//std::size_t const j = (i+1) % 3;
		//std::size_t const k = (i+2) % 3;

		static std::size_t const indices[3] = { 1, 2, 0 };
		std::size_t const j = indices[i];
		std::size_t const k = indices[j];
		
		auto root = std::sqrt(rotation[i][i] - rotation[j][j] - rotation[k][k] + static_cast<T>(1));
		std::array<T*, 3> const quat = {{ &result.X, &result.Y, &result.Z }};
		*quat[i] = half * root;
		root = half / root;
		result.W = (rotation[k][j] - rotation[j][k]) * root;
		*quat[j] = (rotation[j][i] + rotation[i][j]) * root;
		*quat[k] = (rotation[k][i] + rotation[i][k]) * root;
	}
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation, FloatingPointQuaternion & result)
{
	CreateFromRotationMatrixImplementation(rotation, result);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation)
{
	FloatingPointQuaternion result;
	CreateFromRotationMatrix(rotation, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation, FloatingPointQuaternion & result)
{
	CreateFromRotationMatrixImplementation(rotation, result);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation)
{
	FloatingPointQuaternion result;
	CreateFromRotationMatrix(rotation, result);
	return result;
}
//-----------------------------------------------------------------------
template <typename T>
void
FloatingPointQuaternion<T>::CreateFromYawPitchRoll(
	Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll, FloatingPointQuaternion & result)
{
	auto const halfYaw = yaw.value / 2;
	auto const cosYaw = std::cos(halfYaw);
	auto const sinYaw = std::sin(halfYaw);
	
	auto const halfPitch = pitch.value / 2;
	auto const cosPitch = std::cos(halfPitch);
	auto const sinPitch = std::sin(halfPitch);
	
	auto const halfRoll = roll.value / 2;
	auto const cosRoll = std::cos(halfRoll);
	auto const sinRoll = std::sin(halfRoll);

	auto const sinRollCosPitch = sinRoll * cosPitch;
	auto const cosRollSinPitch = cosRoll * sinPitch;
	auto const cosRollCosPitch = cosRoll * cosPitch;
	auto const sinRollSinPitch = sinRoll * sinPitch;

	result.X = (sinRollCosPitch * cosYaw) - (cosRollSinPitch * sinYaw);
	result.Y = (cosRollSinPitch * cosYaw) + (sinRollCosPitch * sinYaw);
	result.Z = (cosRollCosPitch * sinYaw) - (sinRollSinPitch * cosYaw);
	result.W = (cosRollCosPitch * cosYaw) + (sinRollSinPitch * sinYaw);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromYawPitchRoll(
	Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll)
{
	FloatingPointQuaternion result;
	CreateFromYawPitchRoll(yaw, pitch, roll, result);
	return std::move(result);
}
//-----------------------------------------------------------------------
template <typename T>
T const* FloatingPointQuaternion<T>::Data() const
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
T* FloatingPointQuaternion<T>::Data()
{
	static_assert(std::is_floating_point<T>::value, "T is floating point number");
	return &X;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T> operator*(T scaleFactor, FloatingPointQuaternion<T> const& quaternion)
{
	return FloatingPointQuaternion<T>(
		scaleFactor * quaternion.X,
		scaleFactor * quaternion.Y,
		scaleFactor * quaternion.Z,
		scaleFactor * quaternion.W);
}
//-----------------------------------------------------------------------
// explicit instantiations
template class FloatingPointQuaternion<float>;
template FloatingPointQuaternion<float> operator*<float>(float, FloatingPointQuaternion<float> const&);

#if defined(DBL_MANT_DIG)
template class FloatingPointQuaternion<double>;
template FloatingPointQuaternion<double> operator*<double>(double, FloatingPointQuaternion<double> const&);
#endif

#if defined(LDBL_MANT_DIG)
template class FloatingPointQuaternion<long double>;
template FloatingPointQuaternion<long double> operator*<long double>(long double, FloatingPointQuaternion<long double> const&);
#endif

}// namespace Details
}// namespace Pomdog
