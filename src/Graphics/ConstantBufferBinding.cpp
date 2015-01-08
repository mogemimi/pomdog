//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "../RenderSystem/NativeConstantBuffer.hpp"
#include "../RenderSystem/NativeConstantLayout.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Graphics/EffectParameter.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static auto dummyParameter = std::make_shared<EffectParameter>();

}// unnamed namespace
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(GraphicsDevice & graphicsDevice,
	EffectPass & effectPass)
{
	auto nativeDevice = graphicsDevice.NativeGraphicsDevice();

	auto nativeEffectPass = effectPass.NativeEffectPass();

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
	
	nativeConstantLayout = nativeEffectPass->CreateConstantLayout();
	
	// Bind constant buffers:
	for (auto & parameter: effectParameters)
	{
		using Details::RenderSystem::NativeConstantBuffer;
		std::shared_ptr<NativeConstantBuffer> nativeConstantBuffer(
			parameter.second, parameter.second->NativeConstantBuffer());

		POMDOG_ASSERT(nativeConstantLayout);
		nativeConstantLayout->SetConstantBuffer(parameter.first, nativeConstantBuffer);
	}
}
//-----------------------------------------------------------------------
ConstantBufferBinding::ConstantBufferBinding(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	EffectPass & effectPass)
	: ConstantBufferBinding(*graphicsDevice, effectPass)
{
}
//-----------------------------------------------------------------------
ConstantBufferBinding::~ConstantBufferBinding() = default;
//-----------------------------------------------------------------------
std::shared_ptr<EffectParameter> const& ConstantBufferBinding::Find(std::string const& parameterName) const
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
EffectParameterCollection const& ConstantBufferBinding::Find() const
{
	return effectParameters;
}
//-----------------------------------------------------------------------
Details::RenderSystem::NativeConstantLayout* ConstantBufferBinding::NativeConstantLayout()
{
	return nativeConstantLayout.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
