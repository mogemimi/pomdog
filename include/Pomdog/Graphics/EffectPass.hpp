//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP
#define POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {

class NativeEffectPass;

}// namespace RenderSystem
}// namespace Details

class POMDOG_EXPORT EffectPass {
public:
	EffectPass() = delete;
	EffectPass(EffectPass const&) = delete;
	EffectPass(EffectPass &&) = default;
	
	EffectPass(GraphicsDevice & graphicsDevice,
		Details::ShaderBytecode const& vertexShader,
		Details::ShaderBytecode const& pixelShader);
	
	EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
		Details::ShaderBytecode const& vertexShader,
		Details::ShaderBytecode const& pixelShader);

	~EffectPass();
	
	EffectPass & operator=(EffectPass const&) = delete;
	EffectPass & operator=(EffectPass &&) = default;

public:
	Details::RenderSystem::NativeEffectPass* NativeEffectPass();

private:
	std::unique_ptr<Details::RenderSystem::NativeEffectPass> nativeEffectPass;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTPASS_3FA791E9_F535_41AF_B197_3DF71A12085A_HPP)
