//
//  Copyright (C) 2013-2014 mogemimi.
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

#include <memory>
#include <string>
#include <unordered_map>
#include <Pomdog/Config/FundamentalTypes.hpp>
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

using EffectParameterCollection = std::unordered_map<std::string, std::shared_ptr<EffectParameter>>;

class NativeEffectParameter;

class NativeEffectPass
{
public:
	NativeEffectPass() = default;
	NativeEffectPass(NativeEffectPass const&) = delete;
	NativeEffectPass & operator=(NativeEffectPass const&) = delete;

	virtual ~NativeEffectPass() = default;
	
	virtual void SetConstant(std::string const& constantName, std::shared_ptr<NativeEffectParameter> const& effectParameter) = 0;
	
	virtual void SetConstant(std::string const& constantName) = 0;
	
	///@copybrief EffectPass
	virtual void Apply(GraphicsContext & graphicsContext,
		std::shared_ptr<EffectPass> const& sharedThisEffectPass) = 0;
};

}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_RENDERSYSTEM_NATIVEEFFECTPASS_F0C96CAC_A22D_4E8F_A31F_5C2E7A91C273_HPP)
