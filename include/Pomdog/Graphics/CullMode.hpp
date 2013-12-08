//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_CULLMODE_5FBAA280_5F2E_11E3_9361_A8206655A22B_HPP
#define POMDOG_CULLMODE_5FBAA280_5F2E_11E3_9361_A8206655A22B_HPP

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
/// @brief 背面カリングに使用するプリミティブのワインディング順序を定義します。
enum class CullMode: std::uint16_t
{
	///@~Japanese
	/// @brief 常にすべての三角形を描画します。
	None,

	///@~Japanese
	/// @brief ワインディング順序が時計回りの頂点列に対して背面カリングを行います。
	ClockwiseFace,

	///@~Japanese
	/// @brief ワインディング順序が反時計回りの頂点列に対して背面カリングを行います。デフォルト値です。
	CounterClockwiseFace,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_CULLMODE_5FBAA280_5F2E_11E3_9361_A8206655A22B_HPP)
