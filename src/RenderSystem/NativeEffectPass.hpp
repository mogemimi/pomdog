//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP
#define POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <string>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class NativeConstantLayout;
class NativeGraphicsDevice;

class NativeEffectPass {
public:
	NativeEffectPass() = default;
	NativeEffectPass(NativeEffectPass const&) = delete;
	NativeEffectPass & operator=(NativeEffectPass const&) = delete;

	virtual ~NativeEffectPass() = default;

	virtual std::unique_ptr<NativeConstantLayout> CreateConstantLayout() = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP)
