//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CLEAROPTIONS_2A25A8AE_5F15_11E3_A882_A8206655A22B_HPP
#define POMDOG_CLEAROPTIONS_2A25A8AE_5F15_11E3_A882_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"

namespace Pomdog {

enum class ClearOptions: std::uint8_t
{
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

inline constexpr ClearOptions operator|(ClearOptions a, ClearOptions b)
{
	return static_cast<ClearOptions>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

inline ClearOptions & operator|=(ClearOptions & a, ClearOptions b)
{
	static_assert(ClearOptions::ClearOptions_Details_DepthStecil == (
		ClearOptions::DepthBuffer | ClearOptions::Stencil), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetStencil == (
		ClearOptions::RenderTarget | ClearOptions::Stencil), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetDepth == (
		ClearOptions::DepthBuffer | ClearOptions::RenderTarget), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetDepthStencil == (
		ClearOptions::RenderTarget | ClearOptions::DepthBuffer | ClearOptions::Stencil), "");

	a = a | b;
	return a;
}

}// namespace Pomdog

#endif // !defined(POMDOG_CLEAROPTIONS_2A25A8AE_5F15_11E3_A882_A8206655A22B_HPP)
