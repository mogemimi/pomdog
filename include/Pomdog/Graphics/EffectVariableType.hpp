//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP
#define POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief エフェクトパラメータの要素の型を定義します。
enum class EffectVariableType: std::uint8_t {
	///@~Japanese
	/// @brief true または false の 2 値で表される Bool 値です。
	Bool,

	///@~English
	/// @brief 32-bit signed integer.
	///@~Japanese
	/// @brief 32-bit 符号付き整数です。
	Int32,

	///@~English
	/// @brief 32-bit unsigned integer.
	///@~Japanese
	/// @brief 32-bit 符号なし整数です。
	UInt32,

	///@~English
	/// @brief 32-bit floating point value.
	///@~Japanese
	/// @brief 32-bit 浮動小数点値です。
	Float,

	///@~English
	/// @brief 64-bit floating point value.
	///@~Japanese
	/// @brief 64-bit 浮動小数点値です。
	Double,

	///@~Japanese
	/// @brief 文字列です。
	String, 

	///@~Japanese
	/// @brief 1 次元テクスチャです。
	Texture1D,

	///@~Japanese
	/// @brief 2 次元テクスチャです。
	Texture2D,
	
	///@~Japanese
	/// @brief 3 次元テクスチャです。
	Texture3D,

	///@~Japanese
	/// @brief キューブテクスチャです。
	TextureCube,

	///@~Japanese
	/// @brief 型情報を持たない void ポインタです。
	Void,
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTVARIABLETYPE_54C9D81C_73AF_47EF_828C_CAF40803402F_HPP)
