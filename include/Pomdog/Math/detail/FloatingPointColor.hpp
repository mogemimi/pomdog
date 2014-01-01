//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MATH_DETAIL_FLOATINGPOINTCOLOR_248799DB_02C0_4A55_9C07_18225A893ABB_HPP
#define POMDOG_MATH_DETAIL_FLOATINGPOINTCOLOR_248799DB_02C0_4A55_9C07_18225A893ABB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/Export.hpp"
#include "../../Config/FundamentalTypes.hpp"
#include <type_traits>

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief RGBA color.
///@~Japanese
/// @brief 赤、緑、青、およびアルファ（不透明度）の 4 成分を持つ色を定義します。
template <typename T>
class POMDOG_EXPORT FloatingPointColor
{
public:
	static_assert(std::is_floating_point<T>::value, "T is floating point number");

	T r, g, b, a;

public:
	// Constructors:
	FloatingPointColor() = default;
	
	///@brief Copy constructor.
	FloatingPointColor(FloatingPointColor const&) = default;
	
	///@brief Move constructor.
	FloatingPointColor(FloatingPointColor &&) = default;
	
	///@brief Construct from floating-point values.
	FloatingPointColor(T red, T green, T blue, T alpha);

	~FloatingPointColor() = default;

	// Assignment operators:
	FloatingPointColor & operator=(FloatingPointColor const&) = default;
	FloatingPointColor & operator=(FloatingPointColor &&) = default;
	FloatingPointColor & operator+=(FloatingPointColor const&);
	FloatingPointColor & operator-=(FloatingPointColor const&);
	FloatingPointColor & operator*=(T);
	FloatingPointColor & operator/=(T);

	// Unary operators:
	FloatingPointColor operator+() const;
	FloatingPointColor operator-() const;

	// Binary operators:
	FloatingPointColor operator+(FloatingPointColor const&) const;
	FloatingPointColor operator-(FloatingPointColor const&) const;
	FloatingPointColor operator*(T) const;
	FloatingPointColor operator/(T) const;

	bool operator==(FloatingPointColor const&) const;
	bool operator!=(FloatingPointColor const&) const;

	///@~Japanese
	/// @brief 0.0 ~ 1.0 の間に値をクランプします。
	void Saturate();

	///@~Japanese
	/// @brief 指定された2色間を線形補間した色値を作成します。
	/// @param source0 ソースとなる色0
	/// @param source1 ソースとなる色1
	/// @param amount 線形補間する際に使用されるパラメータで、値の範囲に制限はありません。
	static FloatingPointColor CreateLerp(FloatingPointColor const& source0, FloatingPointColor const& source1, T amount);

	static FloatingPointColor CreateHexRGBA(std::uint32_t rgba);
	static FloatingPointColor CreateHexARGB(std::uint32_t argb);

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

	// static public attributes
	static FloatingPointColor White;
	static FloatingPointColor Black;
	static FloatingPointColor Red;
	static FloatingPointColor Green;
	static FloatingPointColor Blue;
	static FloatingPointColor Yellow; //RGBA = (1, 1, 0, 1)
	static FloatingPointColor CornflowerBlue;
	static FloatingPointColor TransparentBlack;//(0, 0, 0, 0)
};

template <typename T>
FloatingPointColor<T> operator*(T scaleFactor, FloatingPointColor<T> const& color);

/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MATH_DETAIL_FLOATINGPOINTCOLOR_248799DB_02C0_4A55_9C07_18225A893ABB_HPP)
