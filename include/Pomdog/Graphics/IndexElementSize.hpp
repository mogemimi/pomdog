//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_INDEXELEMENTSIZE_0E367BDB_845D_45FE_8C3A_D1E82C5E8BDE_HPP
#define POMDOG_INDEXELEMENTSIZE_0E367BDB_845D_45FE_8C3A_D1E82C5E8BDE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief インデックスバッファの要素のビット単位のサイズを定義します。
enum class IndexElementSize: std::uint8_t {
	///@brief 16 bit.
	SixteenBits,
	
	///@brief 32 bit.
	ThirtyTwoBits,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_INDEXELEMENTSIZE_0E367BDB_845D_45FE_8C3A_D1E82C5E8BDE_HPP)
