//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP
#define POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief ブレンディングオプション（ブレンディング係数）を定義します。
enum class Blend: std::uint8_t {
	///@~Japanese
	/// @brief ソースカラーは黒 (0, 0, 0, 0) です。
	Zero,

	///@~Japanese
	/// @brief ソースカラーは白 (1, 1, 1, 1) です。
	One,
	
	///@~Japanese
	/// @brief ソースカラーはピクセルシェーダからのカラーデータです。
	/// 各成分は (Rs, Gs, Bs, As) となります。
	SourceColor,

	///@~Japanese
	/// @brief ソースカラーはピクセルシェーダからのカラーデータです。ブレンディング前に各成分が反転されます。
	/// 各成分は (1, 1, 1, 1) - (Rs, Gs, Bs, As) となります。
	InverseSourceColor,

	///@~Japanese
	/// @brief ソースカラーはピクセルシェーダからのアルファ値です。
	/// 各成分は (As, As, As, As) となります。
	SourceAlpha,

	///@~Japanese
	/// @brief ソースカラーはピクセルシェーダからのアルファ値です。ブレンディング前に各成分が反転されます。
	/// 各成分は (1, 1, 1, 1) - (As, As, As, As) となります。
	InverseSourceAlpha,

	///@~Japanese
	/// @brief ソースカラーはレンダーターゲットからのアルファ値です。
	/// 各成分は (Ad, Ad, Ad, Ad) となります。
	DestinationAlpha,

	///@~Japanese
	/// @brief ソースカラーはレンダーターゲットからのアルファ値です。ブレンディング前に各成分が反転されます。
	/// 各成分は (1, 1, 1, 1) - (Ad, Ad, Ad, Ad) となります。
	InverseDestinationAlpha,

	///@~Japanese
	/// @brief ソースカラーはレンダーターゲットからのカラーデータです。
	/// 各成分は (Rd, Gd, Bd, Ad) となります。
	DestinationColor,

	///@~Japanese
	/// @brief ソースカラーはレンダーターゲットからのカラーデータです。ブレンディング前に各成分が反転されます。
	/// 各成分は (1, 1, 1, 1) - (Rd, Gd, Bd, Ad) となります。
	InverseDestinationColor,

	///@~Japanese
	/// @brief ソースカラーはピクセルシェーダからのアルファ値です。ブレンディング前に各成分が 1 以下にクランプされます。
	/// 各成分は (i, i, i, 1), i = min(As, 1 - Ad)
	SourceAlphaSaturation,

	///@~Japanese
	/// @brief BlendDescription::BlendFactor で設定されたブレンディング係数です。
	/// 各成分は (Rc, Gc, Bc, Ac) となります。
	BlendFactor,

	///@~Japanese
	/// @brief BlendDescription::BlendFactor で設定されたブレンディング係数です。ブレンディング前に各成分が反転されます。
	/// 各成分は (1, 1, 1, 1) - (Rc, Gc, Bc, Ac)
	InvereseBlendFactor,

	//Source1Color,
	//InverseSource1Color,
	//Source1Alpha,
	//InverseSource1Alpha,
};

}// namespace Pomdog

#endif // !defined(POMDOG_BLEND_A27CB239_A90C_46B1_841D_637B0D8343D6_HPP)
