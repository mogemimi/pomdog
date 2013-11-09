//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COLOR_HPP
#define POMDOG_COLOR_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Math
/// @{

///@~English
/// @brief color(RGBA).
///@~Japanese
/// @brief 赤、緑、青、およびアルファ（不透明度）の 4 成分を持つ色を定義します。
class Color
{
private:
	static std::size_t const static_size = 4;

public:
	float r, g, b, a;

public:
	Color(){}
	Color(float red, float green, float blue, float alpha);

	// assignment operators
	Color & operator += (Color const&);
	Color & operator -= (Color const&);
	Color & operator *= (float);
	Color & operator /= (float);

	// unary operators
	Color operator + () const;
	Color operator - () const;

	// binary operators
	Color operator + (Color const&) const;
	Color operator - (Color const&) const;
	Color operator * (float) const;
	Color operator / (float) const;

	friend Color operator *	(float, Color const&);

	bool operator == (Color const&) const;
	bool operator != (Color const&) const;

	// subscript operators
	float& operator[](std::size_t index);
	float  operator[](std::size_t index) const;

	///@~Japanese
	/// @brief 各要素の値を設定します。
	void Set(float red, float green, float blue, float alpha);

	///@~Japanese
	/// @brief 0.0 ~ 1.0 の間に値をクランプします。
	void Saturate();

	///@~Japanese
	/// @brief 指定された2色間を線形補間した色値を作成します。
	/// @param source0 ソースとなる色0
	/// @param source1 ソースとなる色1
	/// @param amount 線形補間する際に使用されるパラメータで、値の範囲に制限はありません。
	static Color CreateLerp(Color const& source0, Color const& source1, float amount);

	static Color CreateHexRGBA(std::uint32_t rgba);
	static Color CreateHexARGB(std::uint32_t argb);

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	float const* Data() const;

	///@~English
	/// @brief Returns pointer to the first element.
	///@~Japanese
	/// @brief 最初の要素へのポインタを返します。
	float* Data();

	// static public attributes
	static Color White;
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Yellow; //RGBA = (1, 1, 0, 1)
	static Color CornflowerBlue;
	static Color TransparentBlack;//(0, 0, 0, 0)
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_COLOR_HPP)
