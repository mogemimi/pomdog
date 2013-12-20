//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/EffectPass.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/EffectParameter.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include "../Utility/MakeUnique.hpp"
#include "../RenderSystem/NativeEffectPass.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static auto dummyParameter = Details::MakeUnique<EffectParameter>();

using Details::RenderSystem::ShaderBytecode;

#define POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(x) \
	"#version 330 \n" + std::string(#x)

std::string const vertexShader = POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(
//====================================================================
layout(location = 0) in vec3 Position;

out VertexData {
	vec4 Position;
} Out;

void main()
{
	Out.Position = vec4(Position.xyz, 1.0);
}
//====================================================================
);

std::string const pixelShader = POMDOG_TOSTRING_SORRY_FOR_USING_MACRO(
//====================================================================
in VertexData {
	vec4 Position;
} In;

out vec4 FragColor;

void main()
{
	FragColor = vec4(In.Position.xy, 1.0, 1.0);
}
//====================================================================
);

}// namespace
//-----------------------------------------------------------------------
EffectPass::EffectPass(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	nativeEffectPass = graphicsDevice->GetNativeGraphicsDevice()->CreateEffectPass(
		ShaderBytecode{
			vertexShader.data(),
			vertexShader.size()
		},
		ShaderBytecode{
			pixelShader.data(),
			pixelShader.size()
		}
	);
}
//-----------------------------------------------------------------------
EffectPass::~EffectPass()
{
}
//-----------------------------------------------------------------------
void EffectPass::Apply()
{
}
//-----------------------------------------------------------------------
std::unique_ptr<EffectParameter> const& EffectPass::Parameters(std::string const& parameterName) const
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
}// namespace Pomdog
