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
#include <Pomdog/Utility/Assert.hpp>
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
		result.x = source.x * InverseLength;
		result.y = source.y * InverseLength;
		result.z = source.z * InverseLength;
		result.w = source.w * InverseLength;
	}
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Normalize(FloatingPointQuaternion const& source)
{
	FloatingPointQuaternion result;
	Normalize(source, result);
	return result;
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
	
	result.x = coefficient1 * begin.x + coefficient2 * end.x;
	result.y = coefficient1 * begin.y + coefficient2 * end.y;
	result.z = coefficient1 * begin.z + coefficient2 * end.z;
	result.w = coefficient1 * begin.w + coefficient2 * end.w;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount)
{
	FloatingPointQuaternion result;
	Slerp(begin, end, amount, result);
	return result;
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
		result.x = source.x * -inverseLengthSquared;
		result.y = source.y * -inverseLengthSquared;
		result.z = source.z * -inverseLengthSquared;
		result.w = source.w *  inverseLengthSquared;
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
	return result;
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

	result.x = axis.x * sinAngle;
	result.y = axis.y * sinAngle;
	result.z = axis.z * sinAngle;
	result.w = cosAngle;
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle)
{
	FloatingPointQuaternion result;
	CreateFromAxisAngle(axis, angle, result);
	return result;
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
		result.w = half * root;
		root = half / root;
		result.x = (rotation[2][1] - rotation[1][2]) * root;
		result.y = (rotation[0][2] - rotation[2][0]) * root;
		result.z = (rotation[1][0] - rotation[0][1]) * root;
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
		std::array<T*, 3> const quat = {{ &result.x, &result.y, &result.z }};
		*quat[i] = half * root;
		root = half / root;
		result.w = (rotation[k][j] - rotation[j][k]) * root;
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
	return result;
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

	result.x = (sinRollCosPitch * cosYaw) - (cosRollSinPitch * sinYaw);
	result.y = (cosRollSinPitch * cosYaw) + (sinRollCosPitch * sinYaw);
	result.z = (cosRollCosPitch * sinYaw) - (sinRollSinPitch * cosYaw);
	result.w = (cosRollCosPitch * cosYaw) + (sinRollSinPitch * sinYaw);
}
//-----------------------------------------------------------------------
template <typename T>
FloatingPointQuaternion<T>
FloatingPointQuaternion<T>::CreateFromYawPitchRoll(
	Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll)
{
	FloatingPointQuaternion result;
	CreateFromYawPitchRoll(yaw, pitch, roll, result);
	return result;
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
template <typename T>
FloatingPointQuaternion<T> operator*(T scaleFactor, FloatingPointQuaternion<T> const& quaternion)
{
	return FloatingPointQuaternion<T>(
		scaleFactor * quaternion.x,
		scaleFactor * quaternion.y,
		scaleFactor * quaternion.z,
		scaleFactor * quaternion.w
	);
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
