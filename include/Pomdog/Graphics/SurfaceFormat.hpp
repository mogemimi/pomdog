//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SURFACEFORMAT_D6D95081_3481_483B_AA66_D40D74FAC014_HPP
#define POMDOG_SURFACEFORMAT_D6D95081_3481_483B_AA66_D40D74FAC014_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief サーフェスフォーマットです。
enum class SurfaceFormat: std::uint8_t
{
	///@~English
	/// @brief 8-bit alpha only.
	///@~Japanese
	/// @brief 8 bit アルファの符号なし整数フォーマット（Unsigned Normalized Intager）です。
	A8_UNorm,

	///@~Japanese
	/// @brief 32 bit の浮動小数点フォーマットです。32 bit の赤のチャンネルのみ使用します。
	R32_Float,

	///@~English
	/// @remarks UNorm means "unsigned normalized" integer.
	///@~Japanese
	/// @brief 4 チャンネル、32 bit の符号なし整数フォーマットです。各チャンネルにそれぞれ 8 bit 使用します。
	R8G8B8A8_UNorm,

	///@~Japanese
	/// @brief 4 チャンネル、128 bit の浮動小数点フォーマットです。各チャンネルにそれぞれ 32 bit 使用します。
	R32G32B32A32_Float,

	///@~Japanese
	/// @brief 0.0 ~ 1.0 の範囲を表す符号なし正規化値の 32 bit フォーマットです。RGB 各チャンネルにそれぞれ 10 bit, Alpha に 2 bit を使用します。
	R10G10B10A2_UNorm,

	///@~Japanese
	/// @brief 2 成分の 32 bit 浮動小数点フォーマットです。1 成分にそれぞれ 16 bit 使用します。
	R16G16_Float,

	///@~English
	/// @remarks UNorm means "unsigned normalized" integer.
	///@~Japanese
	/// @brief 4 チャンネル、32 bit の浮動小数点フォーマットです。各チャンネルにそれぞれ 8 bit 使います。
	B8G8R8A8_UNorm,

	///@~English
	/// @remarks UNorm means "unsigned normalized" integer.
	///@~Japanese
	/// @brief ブロック圧縮フォーマット(DXT1)です。
	BlockComp1_UNorm,

	///@~English
	/// @remarks UNorm means "unsigned normalized" integer.
	///@~Japanese
	/// @brief ブロック圧縮フォーマット(DXT2, DXT3)です。
	BlockComp2_UNorm,

	///@~English
	/// @remarks UNorm means "unsigned normalized" integer.
	///@~Japanese
	/// @brief ブロック圧縮フォーマット(DXT4, DXT5)です。
	BlockComp3_UNorm,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_SURFACEFORMAT_D6D95081_3481_483B_AA66_D40D74FAC014_HPP)
