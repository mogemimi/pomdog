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
#include "../RenderSystem/NativeConstantBuffer.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "EffectConstantDescription.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static auto dummyParameter = std::make_shared<EffectParameter>();
using Details::RenderSystem::NativeConstantBuffer;

}// unnamed namespace
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<GraphicsContext> const& graphicsContextIn,
	Details::ShaderBytecode const& vertexShader, Details::ShaderBytecode const& pixelShader)
	: graphicsContext(graphicsContextIn)
{
	POMDOG_ASSERT(graphicsContextIn);
	POMDOG_ASSERT(!graphicsContext.expired());

	POMDOG_ASSERT(graphicsDevice);
	auto nativeDevice = graphicsDevice->NativeGraphicsDevice();
	
	// Create effect pass:
	POMDOG_ASSERT(nativeDevice);
	nativeEffectPass = nativeDevice->CreateEffectPass(vertexShader, pixelShader);
	
	// Create effect reflection:
	POMDOG_ASSERT(nativeEffectPass);
	auto effectReflection = nativeDevice->CreateEffectReflection(*nativeEffectPass);
	
	POMDOG_ASSERT(effectReflection);
	auto constantBuffers = effectReflection->GetConstantBuffers();
	
	// Create effect parameters:
	for (auto & constantBuffer: constantBuffers)
	{
		auto parameter = std::make_shared<EffectParameter>(graphicsDevice, constantBuffer.ByteSize);
		effectParameters[constantBuffer.Name] = std::move(parameter);
	}
	
	// Bind constant buffers:
	for (auto & parameter: effectParameters)
	{
		std::shared_ptr<NativeConstantBuffer> nativeConstantBuffer(
			parameter.second, parameter.second->NativeConstantBuffer());

		nativeEffectPass->SetConstant(parameter.first, nativeConstantBuffer);
	}
}
//-----------------------------------------------------------------------
EffectPass::~EffectPass() = default;
//-----------------------------------------------------------------------
void EffectPass::Apply()
{
	POMDOG_ASSERT(nativeEffectPass);
	POMDOG_ASSERT(!graphicsContext.expired());
	if (auto sharedContext = graphicsContext.lock())
	{
		POMDOG_ASSERT(shared_from_this());
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
Details::RenderSystem::NativeEffectPass* EffectPass::NativeEffectPass()
{
	return nativeEffectPass.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
