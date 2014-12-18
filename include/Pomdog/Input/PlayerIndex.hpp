//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PLAYERINDEX_FD3F134D_1128_4222_B1FD_0E270A62436B_HPP
#define POMDOG_PLAYERINDEX_FD3F134D_1128_4222_B1FD_0E270A62436B_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief 入力デバイスのプレイヤーを識別する列挙型です。
enum class PlayerIndex: std::uint8_t {
	One = 1,
	Two = 2,
	Three = 3,
	Four = 4,
};

}// namespace Pomdog

#endif // !defined(POMDOG_PLAYERINDEX_FD3F134D_1128_4222_B1FD_0E270A62436B_HPP)
