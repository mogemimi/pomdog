//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_DEPTHSTENCILOPERATION_78E741C0_5F36_11E3_88DD_A8206655A22B_HPP
#define POMDOG_DEPTHSTENCILOPERATION_78E741C0_5F36_11E3_88DD_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "ComparisonFunction.hpp"
#include "StencilOperation.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief ステンシルテストの結果に応じたステンシル処理を記述します。
class DepthStencilOperation
{
public:
	///@~Japanese
	/// @brief ステンシルテストに合格し、深度テストで不合格となった場合に行うステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	StencilOperation StencilDepthBufferFail;

	///@~Japanese
	/// @brief ステンシルテストで不合格となった場合に行うステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	StencilOperation StencilFail;

	///@~Japanese
	/// @brief
	/// ステンシルテストと深度テストの両方に合格した場合に実行するステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	/// @remarks ステンシルテストと深度テストの両方をパスした場合の処理を指定します。
	/// 深度テストをパスしなかった場合に実行するステンシル処理については StencilDepthBufferFail を参照ください。
	StencilOperation StencilPass;

	///@~Japanese
	/// @brief ステンシルテストに用いる比較関数を指定します。
	/// デフォルト値は ComparisonFunction::Always です。
	ComparisonFunction StencilFunction;
	
public:
	DepthStencilOperation()
		: StencilDepthBufferFail(StencilOperation::Keep)
		, StencilFail(StencilOperation::Keep)
		, StencilPass(StencilOperation::Keep)
		, StencilFunction(ComparisonFunction::Always)
	{}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILOPERATION_78E741C0_5F36_11E3_88DD_A8206655A22B_HPP)
