//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectPass.hpp>
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
EffectPass::EffectPass(GraphicsDevice & graphicsDevice,
	Details::ShaderBytecode const& vertexShader, Details::ShaderBytecode const& pixelShader)
{
	auto nativeDevice = graphicsDevice.NativeGraphicsDevice();
	
	// Create effect pass:
	POMDOG_ASSERT(nativeDevice);
	nativeEffectPass = nativeDevice->CreateEffectPass(vertexShader, pixelShader);
}
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	Details::ShaderBytecode const& vertexShader, Details::ShaderBytecode const& pixelShader)
	: EffectPass(*graphicsDevice, vertexShader, pixelShader)
{}
//-----------------------------------------------------------------------
EffectPass::~EffectPass() = default;
//-----------------------------------------------------------------------
Details::RenderSystem::NativeEffectPass* EffectPass::NativeEffectPass()
{
	return nativeEffectPass.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
