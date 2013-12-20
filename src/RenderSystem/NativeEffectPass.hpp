//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP
#define POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeEffectPass
{
public:
	NativeEffectPass() = default;
	NativeEffectPass(NativeEffectPass const&) = delete;
	NativeEffectPass & operator=(NativeEffectPass const&) = delete;

	virtual ~NativeEffectPass() = default;
	
	///@copybrief EffectPass::Apply
	virtual void ApplyShaders() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP)
