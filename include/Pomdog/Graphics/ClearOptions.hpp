// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP
#define POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class ClearOptions: std::uint8_t {
	///@~English
	/// @brief Clear the depth buffer.
	///@~Japanese
	/// @brief 深度バッファをクリアします。
	DepthBuffer = 0b001,

	///@~English
	/// @brief Clear the stencil buffer.
	///@~Japanese
	/// @brief ステンシルバッファをクリアします。
	Stencil = 0b010,

	///@~English
	/// @brief Clear the rendering target.
	///@~Japanese
	/// @brief レンダリングターゲットをクリアします。
	RenderTarget = 0b100,

	///@brief レンダリングターゲット + 深度バッファ + ステンシルバッファ
	ClearOptions_Detail_TargetDepthStencil = 0b001 | 0b010 | 0b100,

	///@brief 深度バッファ + ステンシルバッファ
	ClearOptions_Detail_DepthStecil = 0b001 | 0b010,

	///@brief レンダリングターゲット + ステンシルバッファ
	ClearOptions_Detail_TargetStencil = 0b010 | 0b100,

	///@brief レンダリングターゲット + 深度バッファ
	ClearOptions_Detail_TargetDepth = 0b100 | 0b001,
};

constexpr ClearOptions operator|(ClearOptions a, ClearOptions b)
{
	return static_cast<ClearOptions>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

ClearOptions & operator|=(ClearOptions & a, ClearOptions b);

}// namespace Pomdog

#endif // !defined(POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP)
