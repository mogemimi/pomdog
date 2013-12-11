//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTQUATERNION_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTQUATERNION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/Export.hpp"
#include "../../Config/FundamentalTypes.hpp"
#include "ForwardDeclarations.hpp"
#include "../Radian.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~Japanese
/// @brief 四元数（クォータニオン）を表す 4 次元ベクトルを定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointQuaternion
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	T x, y, z, w;

public:
	// Constructors:
	FloatingPointQuaternion() = default;
	
	///@brief Copy constructor.
	FloatingPointQuaternion(FloatingPointQuaternion const&) = default;
	
	///@brief Move constructor.
	FloatingPointQuaternion(FloatingPointQuaternion &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointQuaternion(T x, T y, T z, T w);
	
	~FloatingPointQuaternion() = default;
	
	// Assignment operators:
	FloatingPointQuaternion & operator=(FloatingPointQuaternion const&) = default;
	FloatingPointQuaternion & operator=(FloatingPointQuaternion &&) = default;
	FloatingPointQuaternion & operator+=(FloatingPointQuaternion const&);
	FloatingPointQuaternion & operator-=(FloatingPointQuaternion const&);
	FloatingPointQuaternion & operator*=(FloatingPointQuaternion const&);
	FloatingPointQuaternion & operator*=(T scaleFactor);
	FloatingPointQuaternion & operator/=(T scaleFactor);

	// Unary operators:
	FloatingPointQuaternion operator+() const;
	FloatingPointQuaternion operator-() const;

	// Binary operators:
	FloatingPointQuaternion operator+(FloatingPointQuaternion const&) const;
	FloatingPointQuaternion operator-(FloatingPointQuaternion const&) const;
	FloatingPointQuaternion operator*(FloatingPointQuaternion const&) const;
	FloatingPointQuaternion operator*(T scaleFactor) const;
	FloatingPointQuaternion operator/(T scaleFactor) const;

	bool operator==(FloatingPointQuaternion const&) const;
	bool operator!=(FloatingPointQuaternion const&) const;

	///@~Japanese
	/// @brief クォータニオンの長さを取得します。
	T Length() const;
	
	///@~Japanese
	/// @brief クォータニオンの長さの 2 乗を計算します。
	T LengthSquared() const;

	///@~Japanese
	/// @brief 2 つのクォータニオンとの内積の値を返します。
	static T Dot(FloatingPointQuaternion const& a, FloatingPointQuaternion const& b);

	///@~Japanese
	/// @brief 単位長に正規化されたクォータニオンを計算します。
	void Normalize();

	///@~Japanese
	/// @brief 単位長に正規化されたクォータニオンを新しく作成します。
	static FloatingPointQuaternion Normalize(FloatingPointQuaternion const& quaternion);
	
	///@~Japanese
	/// @brief 単位長に正規化されたクォータニオンを新しく作成します。
	static void Normalize(FloatingPointQuaternion const& quaternion, FloatingPointQuaternion & result);
	
	///@~Japanese
	/// @brief 2つのクォータニオン間を球面線形補間を使って補間します。
	static void
	Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount, FloatingPointQuaternion & result);
	
	///@~Japanese
	/// @brief 2つのクォータニオン間を球面線形補間を使って補間します。
	static FloatingPointQuaternion
	Slerp(FloatingPointQuaternion const& begin, FloatingPointQuaternion const& end, T amount);
	
	///@~Japanese
	/// @brief 逆クォータニオンを返します。共役クォータニオンを正規化したものを返します。
	static void
	Inverse(FloatingPointQuaternion const& source, FloatingPointQuaternion & result);

	///@~Japanese
	/// @brief 逆クォータニオンを返します。共役クォータニオンを正規化したものを返します。
	static FloatingPointQuaternion
	Inverse(FloatingPointQuaternion const& source);

	///@~Japanese
	/// @brief 指定されたベクトルの周囲を任意の角度で回転するクォータニオンを作成します。正規化されたベクトルを指定してください。
	static void
	CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle, FloatingPointQuaternion & result);

	///@~Japanese
	/// @brief 指定されたベクトルの周囲を任意の角度で回転するクォータニオンを作成します。
	static FloatingPointQuaternion
	CreateFromAxisAngle(FloatingPointVector3<T> const& axis, Radian<T> const& angle);

	///@~English
	/// @brief Convert rotation matrix to quaternion.
	///@~Japanese
	/// @brief 指定された回転行列からクォータニオンを作成します。
	static void
	CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation, FloatingPointQuaternion & result);

	///@~Japanese
	/// @brief 指定された回転行列からクォータニオンを作成します。
	static FloatingPointQuaternion
	CreateFromRotationMatrix(FloatingPointMatrix4x4<T> const& rotation);
	
	///@~English
	/// @brief Convert rotation matrix to quaternion.
	///@~Japanese
	/// @brief 指定された回転行列からクォータニオンを作成します。
	static void
	CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation, FloatingPointQuaternion & result);

	///@~Japanese
	/// @brief 指定された回転行列からクォータニオンを作成します。
	static FloatingPointQuaternion
	CreateFromRotationMatrix(FloatingPointMatrix3x3<T> const& rotation);

	///@~Japanese
	/// @brief 指定されたヨー、ピッチ、ロールのラジアン角からクォータニオンを作成します。
	static void
	CreateFromYawPitchRoll(Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll, FloatingPointQuaternion & result);

	///@~Japanese
	/// @brief 指定されたヨー、ピッチ、ロールのラジアン角からクォータニオンを作成します。
	static FloatingPointQuaternion
	CreateFromYawPitchRoll(Radian<T> const& yaw, Radian<T> const& pitch, Radian<T> const& roll);

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	T const* Data() const;

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	T* Data();
	
	///@~Japanese
	/// @brief 回転しないことを表す単位クォータニオンです。
	/// @remarks それぞれ (x, y, z, w) に対応して、(0, 0, 0, 1) となるクォータニオンです。
	static FloatingPointQuaternion const Identity;
};

template <typename T>
FloatingPointQuaternion<T> operator*(T scaleFactor, FloatingPointQuaternion<T> const& quaternion);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTQUATERNION_HPP)
