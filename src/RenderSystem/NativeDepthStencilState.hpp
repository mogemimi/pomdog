//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_A7B5_401E_970A_65DF722150AB_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_A7B5_401E_970A_65DF722150AB_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDepthStencilState {
public:
	NativeDepthStencilState() = default;
	NativeDepthStencilState(NativeDepthStencilState const&) = delete;
	NativeDepthStencilState & operator=(NativeDepthStencilState const&) = delete;

	virtual ~NativeDepthStencilState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前に深度ステンシルステートを適用します。
	virtual void Apply() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_A8D5CCF3_A7B5_401E_970A_65DF722150AB_HPP)
