//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP
#define POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ComparisonFunction.hpp"
#include "StencilOperation.hpp"

namespace Pomdog {

///@~Japanese
/// @brief ステンシルテストの結果に応じたステンシル処理を記述します。
class DepthStencilOperation {
public:
	///@~Japanese
	/// @brief ステンシルテストに合格し、深度テストで不合格となった場合に行うステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	StencilOperation StencilDepthBufferFail = StencilOperation::Keep;

	///@~Japanese
	/// @brief ステンシルテストで不合格となった場合に行うステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	StencilOperation StencilFail = StencilOperation::Keep;

	///@~Japanese
	/// @brief
	/// ステンシルテストと深度テストの両方に合格した場合に実行するステンシル処理を指定します。
	/// デフォルト値は StencilOperation::Keep です。
	/// @remarks ステンシルテストと深度テストの両方をパスした場合の処理を指定します。
	/// 深度テストをパスしなかった場合に実行するステンシル処理については StencilDepthBufferFail を参照ください。
	StencilOperation StencilPass = StencilOperation::Keep;

	///@~Japanese
	/// @brief ステンシルテストに用いる比較関数を指定します。
	/// デフォルト値は ComparisonFunction::Always です。
	ComparisonFunction StencilFunction = ComparisonFunction::Always;
};

}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILOPERATION_5B7A3783_7AEB_44B7_A592_638B9FA4ED05_HPP)
