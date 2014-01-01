//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP
#define POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP

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
/// @brief 深度ステンシルフォーマットです。
enum class DepthFormat: std::uint8_t
{
	///@~English
	/// @brief 16-bit buffer format. 16 bits for depth.
	Depth16,
	
	///@~English
	/// @brief 24-bit buffer format. 24 bits for depth.
	Depth24,
	
	///@~English
	/// @brief 32-bit buffer format. 24 bits for depth and 8 bits for stencil.
	Depth24Stencil8,
	
	///@~English
	/// @brief Do not use depeh stencil buffer.
	None,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHFORMAT_5FF8041D_B10E_4EBC_A142_2BAAA12C234B_HPP)
