//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BUFFERUSAGE_2EBC62CA_5FBF_11E3_9D25_A8206655A22B_HPP
#define POMDOG_BUFFERUSAGE_2EBC62CA_5FBF_11E3_9D25_A8206655A22B_HPP

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
/// @brief バッファの使用方法を定義します。
enum class BufferUsage: std::uint8_t
{
	Immutable,
	Dynamic,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_BUFFERUSAGE_2EBC62CA_5FBF_11E3_9D25_A8206655A22B_HPP)
