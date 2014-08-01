//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "FXAA.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
FXAA::FXAA(std::shared_ptr<GameHost> const& gameHost)
{
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	
	samplerLinear = SamplerState::CreateLinearWrap(graphicsDevice);
	
	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;
		
		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-1.0f,  1.0f, 0.0f), Vector2(0.0f, 1.0f),
			Vector3( 1.0f,  1.0f, 0.0f), Vector2(1.0f, 1.0f),
			Vector3( 1.0f, -1.0f, 0.0f), Vector2(1.0f, 0.0f),
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			VertexCombined::Declaration().StrideBytes(), BufferUsage::Immutable);

		effectPass = assets->Load<EffectPass>("Effects/FXAA");
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
#ifdef DEBUG
	{
		auto effectReflection = std::make_shared<EffectReflection>(graphicsDevice, effectPass);
	
		auto stream = Log::Stream();
		for (auto & description: effectReflection->GetConstantBuffers()) {
			stream << "-----------------------" << "\n";
			stream << "     Name: " << description.Name << "\n";
			stream << " ByteSize: " << description.ByteSize << "\n";
			stream << "Variables: " << description.Variables.size() << "\n";
		}
	}
#endif
	{
		auto graphicsContext = gameHost->GraphicsContext();
		auto viewport = graphicsContext->Viewport();
		ResetViewportSize(viewport.Bounds);
	}
}
//-----------------------------------------------------------------------
void FXAA::ResetViewportSize(Rectangle const& bounds)
{
	Vector2 renderTargetSize(bounds.Width, bounds.Height);
	constantBuffers->Find("Constants")->SetValue(renderTargetSize);
}
//-----------------------------------------------------------------------
void FXAA::Draw(GraphicsContext & graphicsContext, std::shared_ptr<RenderTarget2D> const& texture)
{
	graphicsContext.SetSamplerState(0, samplerLinear);

	graphicsContext.Clear(Color::CornflowerBlue);
	
	graphicsContext.SetTexture(0, texture);
	graphicsContext.SetInputLayout(inputLayout);
	graphicsContext.SetVertexBuffer(vertexBuffer);
	graphicsContext.SetEffectPass(effectPass);
	graphicsContext.SetConstantBuffers(constantBuffers);
	graphicsContext.DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());
}
//-----------------------------------------------------------------------
}// namespace TestApp
