// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP
#define POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
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
};

constexpr ClearOptions operator|(ClearOptions a, ClearOptions b)
{
	return static_cast<ClearOptions>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

#if __cplusplus < 201402L
POMDOG_EXPORT ClearOptions & operator|=(ClearOptions & a, ClearOptions b);
#else
constexpr ClearOptions & operator|=(ClearOptions & a, ClearOptions b)
{
	a = a | b;
	return a;
}
#endif

}// namespace Pomdog

#endif // !defined(POMDOG_CLEAROPTIONS_F51CCEF3_A5F7_418E_AD9A_35610A589BB1_HPP)
