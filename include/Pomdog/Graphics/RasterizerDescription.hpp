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
class RasterizerDescription final
{
public:
	///@~Japanese
	/// @brief カリングモードを指定します。
	CullMode CullMode;

	///@~Japanese
	/// @brief 三角形の塗りつぶし方法を指定します。
	FillMode FillMode;
	
	///@~Japanese
	/// @brief 深度テストの時にピクセルに加算される深度バイアスです。デフォルト値は 0 です。
	std::int32_t DepthBias;
	
	///@~Japanese
	/// @note 深度バイアスに掛ける係数です。デフォルト値は 0.0f です。
	float SlopeScaledDepthBias;
	
	///@~Japanese
	/// @brief マルチサンプリングのアンチエイリアシングが有効かどうか指定します。デフォルト値は無効 (false) です。
	/// @remarks アンチエイリアシングが有効な場合は true を、無効な場合は false を指定します。
	bool MultisampleEnable;
	
	///@~Japanese
	/// @brief シザーテストが有効かどうかを指定します。デフォルト値は無効 (false) です。
	/// @remarks シザーテストが有効な場合は true を、無効な場合は false を指定します。
	/// @note シザー矩形は GraphicsContext::SetScissorRectangle で指定します。
	bool ScissorTestEnable;
	
public:
	RasterizerDescription()
		: CullMode(CullMode::CounterClockwiseFace)
		, FillMode(FillMode::Solid)
		, DepthBias(0)
		, SlopeScaledDepthBias(0.0f)
		, MultisampleEnable(false)
		, ScissorTestEnable(false)
	{}
};

///	@}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERDESCRIPTION_5D136DA6_5FFF_11E3_9694_A8206655A22B_HPP)
