//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_RASTERIZERDESCRIPTION_5D136DA6_5FFF_11E3_9694_A8206655A22B_HPP
#define POMDOG_RASTERIZERDESCRIPTION_5D136DA6_5FFF_11E3_9694_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/FundamentalTypes.hpp"
#include "CullMode.hpp"
#include "FillMode.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief ラスタライズ処理を記述します。
class RasterizerDescription
{
public:
	///@~Japanese
	/// @brief カリングモードを指定します。
	CullMode cullMode;

	///@~Japanese
	/// @brief 三角形の塗りつぶし方法を指定します。
	FillMode fillMode;

public:
	RasterizerDescription()
		: cullMode(CullMode::CounterClockwiseFace)
		, fillMode(FillMode::Solid)
	{}
};

///	@}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERDESCRIPTION_5D136DA6_5FFF_11E3_9694_A8206655A22B_HPP)
