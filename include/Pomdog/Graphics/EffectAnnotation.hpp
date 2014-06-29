//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP
#define POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "EffectVariableClass.hpp"
#include "EffectVariableType.hpp"

namespace Pomdog {

///@~Japanese
/// @brief エフェクト変数の型を示すメタデータです。
class EffectAnnotation {
public:
	///@~Japanese
	/// @brief エフェクト変数のクラスです。
	EffectVariableClass VariableClass;

	///@~Japanese
	/// @brief エフェクト変数の型です。
	EffectVariableType VariableType;

	///@~Japanese
	/// @brief 列の数です。
	std::uint8_t ColumnCount;

	///@~Japanese
	/// @brief 行の数です。
	std::uint8_t RowCount;
	
	///@~Japanese
	/// @brief 配列の要素数です。配列を持たない場合は 0 となります。
	std::uint8_t Elements;
	//std::optional<std::uint8_t> Elements;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP)
