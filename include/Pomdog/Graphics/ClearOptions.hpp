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
	DepthBuffer = 0x1,

	///@~English
	/// @brief Clear the stencil buffer.
	///@~Japanese
	/// @brief ステンシルバッファをクリアします。
	Stencil = 0x2,

	///@~English
	/// @brief Clear the rendering target.
	///@~Japanese
	/// @brief レンダリングターゲットをクリアします。
	RenderTarget = 0x4,

	// Details:

	///@brief レンダリングターゲット + 深度バッファ + ステンシルバッファ
	ClearOptions_Details_TargetDepthStencil = 0x1 | 0x2 | 0x4,

	///@brief 深度バッファ + ステンシルバッファ
	ClearOptions_Details_DepthStecil = 0x1 | 0x2,

	///@brief レンダリングターゲット + ステンシルバッファ
	ClearOptions_Details_TargetStencil = 0x2 | 0x4,

	///@brief レンダリングターゲット + 深度バッファ
	ClearOptions_Details_TargetDepth = 0x4 | 0x1,
};

constexpr ClearOptions operator|(ClearOptions a, ClearOptions b)
{
	return static_cast<ClearOptions>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

ClearOptions & operator|=(ClearOptions & a, ClearOptions b);

}// namespace Pomdog

#endif // !defined(POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP)
