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
#include "Pomdog/Graphics/CustomVertex.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectParameter.hpp"
#include "Pomdog/Graphics/GraphicsContext.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/InputLayout.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/SamplerState.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>

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
	samplerLinear = SamplerState::CreateLinearWrap(graphicsDevice);
	
	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;
		
		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3{-1.0f, -1.0f, 0.0f}, Vector2{0.0f, 0.0f},
			Vector3{-1.0f,  1.0f, 0.0f}, Vector2{0.0f, 1.0f},
			Vector3{ 1.0f,  1.0f, 0.0f}, Vector2{1.0f, 1.0f},
			Vector3{ 1.0f, -1.0f, 0.0f}, Vector2{1.0f, 0.0f},
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			VertexCombined::Declaration().StrideBytes(), BufferUsage::Immutable);
	}
	{
		effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectFxaaTrait>(*graphicsDevice);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
			0, 1, 2,
			2, 3, 0
		};

		// Create index buffer
		indexBuffer = std::make_shared<IndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size(), BufferUsage::Immutable);
	}
//#ifdef DEBUG
//	{
//		auto effectReflection = std::make_shared<EffectReflection>(graphicsDevice, effectPass);
//	
//		auto stream = Log::Stream();
//		for (auto & description: effectReflection->GetConstantBuffers()) {
//			stream << "-----------------------" << "\n";
//			stream << "     Name: " << description.Name << "\n";
//			stream << " ByteSize: " << description.ByteSize << "\n";
//			stream << "Variables: " << description.Variables.size() << "\n";
//		}
//	}
//#endif
}
//-----------------------------------------------------------------------
void FXAA::SetViewport(float width, float height)
{
	Vector2 renderTargetSize(width, height);
	constantBuffers->Find("Constants")->SetValue(renderTargetSize);
}
//-----------------------------------------------------------------------
void FXAA::SetTexture(std::shared_ptr<RenderTarget2D> const& textureIn)
{
	POMDOG_ASSERT(textureIn);
	texture = textureIn;
}
//-----------------------------------------------------------------------
void FXAA::Draw(GraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(texture);

	graphicsContext.SetSamplerState(0, samplerLinear);
	graphicsContext.SetTexture(0, texture);
	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	graphicsContext.SetEffectPass(effectPass);
	graphicsContext.SetConstantBuffers(constantBuffers);
	graphicsContext.DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());
}
//-----------------------------------------------------------------------
}// namespace Pomdog
