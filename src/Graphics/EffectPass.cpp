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
#include "../RenderSystem/EffectBufferDescription.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeEffectParameter.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/NativeShaderReflection.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static auto dummyParameter = std::make_shared<EffectParameter>();

using Details::RenderSystem::ShaderBytecode;
using Details::RenderSystem::NativeEffectParameter;

#define POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(x) \
	"#version 330 \n" + std::string(#x)

std::string const vertexShader = POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(
//====================================================================
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TextureCoord;

out VertexData {
	vec2 TextureCoord;
} Out;

void main()
{
	gl_Position = vec4(Position.xyz, 1.0);
	Out.TextureCoord = TextureCoord.xy;
}
//====================================================================
);

std::string const pixelShader = POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(
//====================================================================
in VertexData {
	vec2 TextureCoord;
} In;

uniform Material {
	vec4 Ambient;
	vec4 Diffuse[2];
	vec3 Specular;
	float Emissive[4];
};

uniform ViewProjection {
	mat3 View;
	mat4 Projection;
};

out vec4 FragColor;

void main()
{
	FragColor = vec4(In.TextureCoord.xy, 1.0, 1.0)
	* Ambient * Diffuse[0].r * Specular.r * Emissive[3] * View[0].x * Projection[0].y; // Debug code
}
//====================================================================
);

}// namespace
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	std::shared_ptr<GraphicsContext> const& graphicsContextIn)
	: graphicsContext(graphicsContextIn)
{
	POMDOG_ASSERT(graphicsContextIn);
	POMDOG_ASSERT(!this->graphicsContext.expired());

	auto nativeDevice = graphicsDevice->GetNativeGraphicsDevice();

	nativeEffectPass = nativeDevice->CreateEffectPass(
		ShaderBytecode{
			vertexShader.data(),
			vertexShader.size()
		},
		ShaderBytecode{
			pixelShader.data(),
			pixelShader.size()
		}
	);
	
	// Create shader reflection:
	POMDOG_ASSERT(nativeEffectPass);
	auto shaderReflection = nativeDevice->CreateShaderReflection(*nativeEffectPass);
	
	POMDOG_ASSERT(shaderReflection);
	auto constantBuffers = shaderReflection->GetConstantBuffers();
	
	// Create effect parameters:
	for (auto & constantBuffer: constantBuffers)
	{
		auto effectParameter = std::make_shared<EffectParameter>(graphicsDevice, constantBuffer.ByteConstants);
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
