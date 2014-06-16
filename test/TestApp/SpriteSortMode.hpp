//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP
#define POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief バッチレンダリングのソート方法を定義します。
enum class SpriteSortMode: std::uint8_t {
	///@~Japanese
	/// @brief 並び変えず、追加された順に描画します。
	Deferred,

	///@~Japanese
	/// @brief 深度値が奥から前になるように並び変え、描画します。
	BackToFront,

	///@~Japanese
	/// @brief 深度値が前から奥になるように並び変え、描画します。
	FrontToBack,
};

}// namespace Pomdog

#endif // !defined(POMDOG_SPRITESORTMODE_670C0FC1_7802_46A0_BF22_C8C399025B2B_HPP)
