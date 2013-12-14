//
//  Copyright (C) 2013 mogemimi.
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

#include "EffectParameterClass.hpp"
#include "EffectParameterType.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

///@~Japanese
/// @brief エフェクトパラメータの要素の型を示すメタデータです。
class EffectAnnotation
{
public:
	///@~Japanese
	/// @brief 列の数です。
	std::uint16_t ColumnCount;

	///@~Japanese
	/// @brief 行の数です。
	std::uint16_t RowCount;
	
	///@~Japanese
	/// @brief パラメータのクラスです。
	EffectParameterClass ParameterClass;

	///@~Japanese
	/// @brief パラメータの型です。
	EffectParameterType ParameterType;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP)
