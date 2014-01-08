//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectPass.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/EffectParameter.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeEffectParameter.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "EffectConstantDescription.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static auto dummyParameter = std::make_shared<EffectParameter>();
using Details::RenderSystem::NativeEffectParameter;

}// namespace
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<GraphicsContext> const& graphicsContextIn,
	Details::ShaderBytecode const& vertexShader, Details::ShaderBytecode const& pixelShader)
	: graphicsContext(graphicsContextIn)
{
	POMDOG_ASSERT(graphicsContextIn);
	POMDOG_ASSERT(!this->graphicsContext.expired());

	auto nativeDevice = graphicsDevice->GetNativeGraphicsDevice();

	nativeEffectPass = nativeDevice->CreateEffectPass(vertexShader, pixelShader);
	
	// Create effect reflection:
	POMDOG_ASSERT(nativeEffectPass);
	auto effectReflection = nativeDevice->CreateEffectReflection(*nativeEffectPass);
	
	POMDOG_ASSERT(effectReflection);
	auto constantBuffers = effectReflection->GetConstantBuffers();
	
	// Create effect parameters:
	for (auto & constantBuffer: constantBuffers)
	{
		auto effectParameter = std::make_shared<EffectParameter>(graphicsDevice, constantBuffer.ByteSize);
		effectParameters[constantBuffer.Name] = std::move(effectParameter);
	}
	
	// Bind constant buffers:
	for (auto & parameter: effectParameters)
	{
		std::shared_ptr<NativeEffectParameter> nativeEffectParameter(
			parameter.second, parameter.second->GetNativeEffectParameter());

		nativeEffectPass->SetConstant(parameter.first, nativeEffectParameter);
	}
}
//-----------------------------------------------------------------------
EffectPass::~EffectPass()
{
	effectParameters.clear();
	nativeEffectPass.reset();
}
//-----------------------------------------------------------------------
void EffectPass::Apply()
{
	POMDOG_ASSERT(nativeEffectPass);
	POMDOG_ASSERT(!graphicsContext.expired());
	if (auto sharedContext = graphicsContext.lock())
	{
		nativeEffectPass->Apply(*sharedContext, shared_from_this());
	}
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectParameter> const& EffectPass::Parameters(std::string const& parameterName) const
{
	POMDOG_ASSERT(!parameterName.empty());
	POMDOG_ASSERT(!effectParameters.empty());
	
	auto iter = effectParameters.find(parameterName);
	if (iter != std::end(effectParameters)) {
		return iter->second;
	}
	return dummyParameter;
}
//-----------------------------------------------------------------------
EffectParameterCollection const& EffectPass::Parameters() const
{
	return effectParameters;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeEffectPass* EffectPass::GetNativeEffectPass()
{
	return this->nativeEffectPass.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
