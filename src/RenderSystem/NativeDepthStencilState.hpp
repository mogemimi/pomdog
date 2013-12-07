//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_8165EFDE_5F33_11E3_BC96_A8206655A22B_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_8165EFDE_5F33_11E3_BC96_A8206655A22B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Noncopyable.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeDepthStencilState: Noncopyable
{
public:
	virtual ~NativeDepthStencilState() = default;
	
	///@~Japanese
	/// @brief レンダリングの直前に深度ステンシルステートを適用します。
	virtual void Apply() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEDEPTHSTENCILSTATE_8165EFDE_5F33_11E3_BC96_A8206655A22B_HPP)
