//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR2_714DEF9F_48AC_43E8_BC90_E0C400E81326_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR2_714DEF9F_48AC_43E8_BC90_E0C400E81326_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <cfloat>
#include <type_traits>
#include "../../Config/Export.hpp"
#include "ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief vector in two-dimensional space.
///@~Japanese
/// @brief 2 つの要素を持つベクトル（2 次元ベクトル）を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointVector2 {
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	T X, Y;

public:
	// Constructors:
	FloatingPointVector2() = default;
	
	///@brief Copy constructor.
	FloatingPointVector2(FloatingPointVector2 const&) = default;
	
	///@brief Move constructor.
	FloatingPointVector2(FloatingPointVector2 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointVector2(T x, T y);
	
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
	/// @brief 2 つのベクトル間の距離の 2 乗を計算します。
	static T DistanceSquared(FloatingPointVector2 const& a, FloatingPointVector2 const& b);
	
	///@~Japanese
	/// @brief 2 つのベクトルの内積を計算します。
	static T Dot(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

	///@~Japanese
	/// @brief 2 つのベクトルの外積を計算します。
	static T Cross(FloatingPointVector2 const& a, FloatingPointVector2 const& b);

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

	///@~Japanese
	/// @brief 2 つのベクトル間を線形補間します。
	/// @param source1 ソースとなるベクトル1
	/// @param source2 ソースとなるベクトル2
	/// @param amount 線形補間する際に使用されるパラメータで、値の範囲に制限はありません。
	static FloatingPointVector2 Lerp(FloatingPointVector2 const& source1, FloatingPointVector2 const& source2, T amount);

	///@~Japanese
	/// @brief 2 つのベクトル間をスムーズに補間します。
	/// @details 返却値は次のベクトルと同じです。
	/// FloatingPointVector2 result;
	/// result.x = MathHelper::SmoothStep(source1.x, source2.x, amount);
	/// result.y = MathHelper::SmoothStep(source1.y, source2.y, amount);
	static FloatingPointVector2 SmoothStep(FloatingPointVector2 const& source1, FloatingPointVector2 const& source2, T amount);

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
	
	///@note (0, 0, 0)
	static FloatingPointVector2 const Zero;
};

template <typename T>
FloatingPointVector2<T> operator*(T scaleFactor, FloatingPointVector2<T> const& vector);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR2_714DEF9F_48AC_43E8_BC90_E0C400E81326_HPP)
