//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_276ECA57_5FFF_11E3_B28E_A8206655A22B_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_276ECA57_5FFF_11E3_B28E_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Platform.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeRasterizerState
{
public:
	NativeRasterizerState() = default;
	NativeRasterizerState(NativeRasterizerState const&) = delete;
	NativeRasterizerState& operator=(NativeRasterizerState const&) = delete;
	
	virtual ~NativeRasterizerState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前にラスタライザステートを適用します。
	virtual void Apply() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVERASTERIZERSTATE_276ECA57_5FFF_11E3_B28E_A8206655A22B_HPP)
