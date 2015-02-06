//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP
#define POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief エフェクトパラメータの要素のクラスを定義します。
enum class EffectVariableClass: std::uint8_t {
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

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTVARIABLECLASS_0B44E0FB_0B71_45D3_9981_8089B38B8655_HPP)
