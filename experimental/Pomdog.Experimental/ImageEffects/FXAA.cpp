//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "FXAA.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"
#include "Pomdog/Graphics/ConstantBufferBinding.hpp"
#include "Pomdog/Graphics/VertexDeclaration.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectParameter.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/InputLayout.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/FXAA_VS.inc.h"
#include "Shaders/GLSL.Embedded/FXAA_PS.inc.h"

struct BuiltinEffectFxaaTrait {
	static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
	{
		using Details::ShaderBytecode;
		ShaderBytecode vertexShaderCode = {Builtin_GLSL_FXAA_VS, std::strlen(Builtin_GLSL_FXAA_VS)};
		ShaderBytecode pixelShaderCode = {Builtin_GLSL_FXAA_PS, std::strlen(Builtin_GLSL_FXAA_PS)};
		return std::make_shared<EffectPass>(graphicsDevice, vertexShaderCode, pixelShaderCode);
	}
};

}// unnamed namespace
//-----------------------------------------------------------------------
FXAA::FXAA(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
{
	samplerLinear = SamplerState::CreateLinearClamp(graphicsDevice);

	effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectFxaaTrait>(*graphicsDevice);
	constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
	inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass,
		VertexDeclaration{{VertexElementFormat::Float3, VertexElementFormat::Float2}});
}
//-----------------------------------------------------------------------
void FXAA::SetViewport(float width, float height)
{
	Vector2 rcpFrame = Vector2(1.0f, 1.0f) / Vector2(width, height);
	constantBuffers->Find("FxaaPassBuffer")->SetValue(rcpFrame);
}
//-----------------------------------------------------------------------
void FXAA::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
	POMDOG_ASSERT(textureIn);
	texture = textureIn;
}
//-----------------------------------------------------------------------
void FXAA::Apply(GraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(texture);

	graphicsContext.SetSamplerState(0, samplerLinear);
	graphicsContext.SetTexture(0, texture);
	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetEffectPass(effectPass);
	graphicsContext.SetConstantBuffers(constantBuffers);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
