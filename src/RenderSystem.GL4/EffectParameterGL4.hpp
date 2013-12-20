//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP
#define POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../RenderSystem/NativeEffectParameter.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class EffectParameterGL4: public NativeEffectParameter
{
public:
	///@~Japanese
	/// @brief 現在有効になっているシェーダプログラムにコンスタントバッファを適用します。
	virtual void ApplyConstant() = 0;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_EFFECTPARAMETERGL4_477143C6_EA0B_4118_95F5_1F606E7063A9_HPP)
