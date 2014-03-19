//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR3_D6230CBF_0247_4B72_BDB8_C7FD3C47A0CB_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR3_D6230CBF_0247_4B72_BDB8_C7FD3C47A0CB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <type_traits>
#include "../../Config/Export.hpp"
#include "../../Config/FundamentalTypes.hpp"
#include "ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief vector in three-dimensional space.
///@~Japanese
/// @brief 3 つの要素を持つベクトル（3 次元ベクトル）を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointVector3 {
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point.");
	typedef T value_type;

	T X, Y, Z;

public:
	// Constructors:
	FloatingPointVector3() = default;
	
	///@brief Copy constructor.
	FloatingPointVector3(FloatingPointVector3 const&) = default;
	
	///@brief Move constructor.
	FloatingPointVector3(FloatingPointVector3 &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointVector3(T x, T y, T z);
	FloatingPointVector3(FloatingPointVector2<T> const& vec, T z);
	
	~FloatingPointVector3() = default;
	
	// Assignment operators:
	FloatingPointVector3 & operator=(FloatingPointVector3 const&) = default;
	FloatingPointVector3 & operator=(FloatingPointVector3 &&) = default;
	FloatingPointVector3 & operator+=(FloatingPointVector3 const&);
	FloatingPointVector3 & operator-=(FloatingPointVector3 const&);
	FloatingPointVector3 & operator*=(T);
	FloatingPointVector3 & operator/=(T);

	// Unary operators:
	FloatingPointVector3 operator+() const;
	FloatingPointVector3 operator-() const;

	// Binary operators:
	FloatingPointVector3 operator+(FloatingPointVector3 const&) const;
	FloatingPointVector3 operator-(FloatingPointVector3 const&) const;
	FloatingPointVector3 operator*(FloatingPointVector3 const&) const;
	FloatingPointVector3 operator/(FloatingPointVector3 const&) const;
	FloatingPointVector3 operator*(T scaleFactor) const;
	FloatingPointVector3 operator/(T scaleFactor) const;

	bool operator==(FloatingPointVector3 const& other) const;
	bool operator!=(FloatingPointVector3 const& other) const;

	///@~Japanese
	/// @brief ベクトルの長さを計算します。
	T Length() const;
	
	///@~Japanese
	/// @brief ベクトルの長さの 2 乗を計算します。
	T LengthSquared() const;
	
	///@~Japanese
	/// @brief 2 つのベクトル間の距離を計算します。
	static T Distance(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 2 つのベクトル間の距離の 2 乗を計算します。
	static T DistanceSquared(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 2 つのベクトルの内積を計算します。
	static T Dot(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 2 つのベクトルの外積を計算します。
	static FloatingPointVector3 Cross(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 2 つのベクトルが含む成分の最小値で構成されたベクトルを返します。
	/// @details 返却値は次のベクトルと同じです。
	/// FloatingPointVector3 result;
	/// result.x = std::min(a.x, b.x);
	/// result.y = std::min(a.y, b.y);
	/// result.z = std::min(a.z, b.z);
	static FloatingPointVector3 Min(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 2 つのベクトルが含む成分の最大値で構成されたベクトルを返します。
	/// @details 返却値は次のベクトルと同じです。
	/// FloatingPointVector3 result;
	/// result.x = std::max(a.x, b.x);
	/// result.y = std::max(a.y, b.y);
	/// result.z = std::max(a.z, b.z);
	static FloatingPointVector3 Max(FloatingPointVector3 const& a, FloatingPointVector3 const& b);

	///@~Japanese
	/// @brief 指定された範囲内にベクトルをクランプします。
	/// @param source クランプするベクトル
	/// @param min 最小値
	/// @param max 最大値
	/// @details 返却値は次のベクトルと同じです。
	/// FloatingPointVector3 result;
	/// result.x = MathHelper::Clamp(source.x, min.x, max.x);
	/// result.y = MathHelper::Clamp(source.y, min.y, max.y);
	/// result.z = MathHelper::Clamp(source.z, min.z, max.z);
	static FloatingPointVector3 Clamp(FloatingPointVector3 const& source,
		FloatingPointVector3 const& min, FloatingPointVector3 const& max);

	///@~Japanese
	/// @brief 2 つのベクトル間のスムーズな補間を計算します。
	/// @details 返却値は次のベクトルと同じです。
	/// FloatingPointVector3 result;
	/// result.x = MathHelper::SmoothStep(a.x, b.x, amount);
	/// result.y = MathHelper::SmoothStep(a.y, b.y, amount);
	/// result.z = MathHelper::SmoothStep(a.z, b.z, amount);
	static FloatingPointVector3 SmoothStep(FloatingPointVector3 const& a, FloatingPointVector3 const& b, T amount);

	///@~Japanese
	/// @brief 現在のベクトルを単位ベクトルに変換します。
	void Normalize();

	///@~Japanese
	/// @brief 指定されたベクトルの単位ベクトルを計算します。
	static FloatingPointVector3 Normalize(FloatingPointVector3 const& source);

	///@~Japanese
	/// @brief 指定されたベクトルの単位ベクトルを計算します。
	static void Normalize(FloatingPointVector3 const&, FloatingPointVector3 & result);
	
	///@~Japanese
	/// @brief 指定された行列を使用して 3 次元ベクトルをトランスフォームします。
	/// @param position 元となる 3 次元ベクトル
	/// @param matrix 4x4 のトランスフォーム行列
	/// @return トランスフォームされた 3 次元ベクトル
	static FloatingPointVector3 Transform(FloatingPointVector3 const& position, FloatingPointMatrix4x4<T> const& matrix);

	///@~Japanese
	/// @brief 指定された行列を使用して法線ベクトルをトランスフォームします。
	static FloatingPointVector3 TransformNormal(FloatingPointVector3 const& normal, FloatingPointMatrix4x4<T> const& matrix);

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
	static FloatingPointVector3 const Zero;

	///@note (1, 0, 0)
	static FloatingPointVector3 const UnitX;

	///@note (0, 1, 0)
	static FloatingPointVector3 const UnitY;

	///@note (0, 0, 1)
	static FloatingPointVector3 const UnitZ;
};

template <typename T>
FloatingPointVector3<T> operator*(T scaleFactor, FloatingPointVector3<T> const& vector);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTVECTOR3_D6230CBF_0247_4B72_BDB8_C7FD3C47A0CB_HPP)
