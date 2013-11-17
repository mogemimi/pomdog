//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DETAIL_FLOATINGPOINTVECTOR2_HPP
#define POMDOG_DETAIL_FLOATINGPOINTVECTOR2_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/FundamentalTypes.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

template <typename T>
class FloatingPointMatrix4x4;

template <typename T>
class FloatingPointQuaternion;

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief vector in two-dimensional space.
///@~Japanese
/// @brief 2 つの要素を持つベクトル（2 次元ベクトル）を定義します。
template <typename T>
class FloatingPointVector2
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	T x, y;

public:
	// Constructors:
	FloatingPointVector2() = default;
	
	///@brief Copy constructor.
	FloatingPointVector2(FloatingPointVector2 const&) = default;
	
	///@brief Move constructor.
	FloatingPointVector2(FloatingPointVector2 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointVector2(T x, T y);
	
	virtual ~FloatingPointVector2() = default;
	
	// Assignment operators:
	FloatingPointVector2 & operator=(FloatingPointVector2 const&) = default;
	FloatingPointVector2 & operator=(FloatingPointVector2 &&) = default;
	FloatingPointVector2 & operator+=(FloatingPointVector2 const&);
	FloatingPointVector2 & operator-=(FloatingPointVector2 const&);
	FloatingPointVector2 & operator*=(T);
	FloatingPointVector2 & operator/=(T);

	// Unary operators:
	FloatingPointVector2 operator+() const;
	FloatingPointVector2 operator-() const;

	// Binary operators:
	FloatingPointVector2 operator+(FloatingPointVector2 const&) const;
	FloatingPointVector2 operator-(FloatingPointVector2 const&) const;
	FloatingPointVector2 operator*(FloatingPointVector2 const&) const;
	FloatingPointVector2 operator/(FloatingPointVector2 const&) const;
	
	FloatingPointVector2 operator*(T scaleFactor) const;
	FloatingPointVector2 operator/(T scaleFactor) const;

	bool operator==(FloatingPointVector2 const& other) const;
	bool operator!=(FloatingPointVector2 const& other) const;

	// Products, length, and normalize:
	
	///@~Japanese
	/// @brief ベクトルの長さを計算します。
	T Length() const;
	
	///@~Japanese
	/// @brief ベクトルの長さの 2 乗を計算します。
	T LengthSquared() const;
	
	///@~Japanese
	/// @brief 2 つのベクトル間の距離を計算します。
	static T Distance(FloatingPointVector2 const& a, FloatingPointVector2 const& b);
	
	///@~Japanese
	/// @brief 2 つのベクトルの内積を計算します。
	static T Dot(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

	///@~Japanese
	/// @brief 現在のベクトルを単位ベクトルに変換します。
	void Normalize();
	
	///@~Japanese
	/// @brief 指定されたベクトルの単位ベクトルを計算します。
	static FloatingPointVector2 Normalize(FloatingPointVector2 const& source);
	
	///@~Japanese
	/// @brief 指定されたベクトルの単位ベクトルを計算します。
	static void Normalize(FloatingPointVector2 const& source, FloatingPointVector2 & result);

	///@~Japanese
	/// @brief 指定された行列を使用してベクトルをトランスフォームします。
	static FloatingPointVector2 Transform(FloatingPointVector2 const& position, FloatingPointMatrix4x4<T> const& matrix);

	///@~Japanese
	/// @brief 指定された Quaternion を使用してベクトルをトランスフォームします。
	static FloatingPointVector2 Transform(FloatingPointVector2 const& position, FloatingPointQuaternion<T> const& rotation);

	///@~Japanese
	/// @brief 指定された行列を使用して法線ベクトルをトランスフォームします。
	static FloatingPointVector2 TransformNormal(FloatingPointVector2 const& normal, FloatingPointMatrix4x4<T> const& matrix);

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
};

template <typename T>
FloatingPointVector2<T> operator*(T scaleFactor, FloatingPointVector2<T> const& vector);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DETAIL_FLOATINGPOINTVECTOR2_HPP)
