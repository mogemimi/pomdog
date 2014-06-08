//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP
#define POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class ContainmentType: std::uint8_t {
	///@~Japanese
	/// @brief 境界ボリュームが別の境界ボリュームを完全に内包していることを示します。
	Contains,

	///@~Japanese
	/// @brief 境界ボリューム同士が交差していないことを示します。
	Disjoint,

	///@~Japanese
	/// @brief 境界ボリューム同士が部分的に交差していることを示します。
	Intersects
};

}// namespace Pomdog

#endif // !defined(POMDOG_CONTAINMENTTYPE_21BFFFDA_D1CD_4222_B556_0EFF89954D5D_HPP)
