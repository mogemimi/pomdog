//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTPARAMETERCLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP
#define POMDOG_EFFECTPARAMETERCLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP

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
/// @brief エフェクトパラメータの要素のクラスを定義します。
enum class EffectParameterClass: std::uint8_t
{
	///@~Japanese スカラーです。
	Scalar,

	///@~Japanese ベクトルです。
	Vector,

	///@~Japanese 行列です。
	Matrix, 

	///@~Japanese テクスチャオブジェクトまたはシェーダ、あるいは文字列オブジェクトです。
	Object,

	///@~Japanese ユーザ定義の構造体です。
	Struct,
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPARAMETERCLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP)
