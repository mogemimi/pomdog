//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP
#define POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/FundamentalTypes.hpp"
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
class FloatingPointQuaternion
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
	
	virtual ~FloatingPointQuaternion() = default;
	
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

	friend FloatingPointQuaternion operator*(T scaleFactor, FloatingPointQuaternion const& quaternion);

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
	/// @brief それぞれ (x, y, z, w) に対応して、(0, 0, 0, 0) となるクォータニオンです。
	static FloatingPointQuaternion const Zero;
};

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_FLOATINGPOINTQUATERNION_HPP)
