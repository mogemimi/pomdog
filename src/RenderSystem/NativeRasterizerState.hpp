//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeGraphicsContext;

class NativeRasterizerState {
public:
	NativeRasterizerState() = default;
	NativeRasterizerState(NativeRasterizerState const&) = delete;
	NativeRasterizerState & operator=(NativeRasterizerState const&) = delete;
	
	virtual ~NativeRasterizerState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前にラスタライザステートを適用します。
	virtual void Apply(NativeGraphicsContext & graphicsContext) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_F1119CE4_3026_4F9A_89AE_C5B035A87704_HPP)
