//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COMPARISONFUNCTION_184E5BB8_5F37_11E3_9985_A8206655A22B_HPP
#define POMDOG_COMPARISONFUNCTION_184E5BB8_5F37_11E3_9985_A8206655A22B_HPP

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
/// @brief 深度バッファテストまたはアルファステンシルテストで用いる比較関数を定義します。
enum class ComparisonFunction: std::uint16_t
{
	///@~Japanese
	/// @brief テストは常に失敗します。
	Never,

	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータ未満の場合、テストに合格します。
	Less,
	
	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータと等しい場合、テストに合格します。
	Equal,

	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータ以下の場合、テストに合格します。
	LessEqual,

	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータを超える場合、テストに合格します。
	Greater,

	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータと等しくない場合、テストに合格します。
	NotEqual,

	///@~Japanese
	/// @brief ソースとなるピクセルデータが対象となるピクセルデータ以上の場合、テストに合格します。
	GreaterEqual,

	///@~Japanese
	/// @brief テストは常に合格します。
	Always,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_COMPARISONFUNCTION_184E5BB8_5F37_11E3_9985_A8206655A22B_HPP)
