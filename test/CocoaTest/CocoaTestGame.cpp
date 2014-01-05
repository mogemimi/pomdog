//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "CocoaTestGame.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
CocoaTestGame::CocoaTestGame(std::shared_ptr<GameHost> host)
	: gameHost(std::move(host))
{
	graphicsContext = gameHost->GetGraphicsContext();
}
//-----------------------------------------------------------------------
void CocoaTestGame::Initialize()
{
	auto window = gameHost->GetWindow();
	window->SetCaption("Cocoa Test Game");
	window->SetAllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GetGraphicsDevice();
	
	{
		auto blendState = BlendState::CreateOpaque(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
	}
	{
		auto depthStencilState = DepthStencilState::CreateReadWriteDepth(graphicsDevice);
		graphicsContext->SetDepthStencilState(depthStencilState);
	}
	{
		auto rasterizerState = RasterizerState::CreateCullCounterClockwise(graphicsDevice);
		graphicsContext->SetRasterizerState(rasterizerState);
	}
	{
		auto samplerState = SamplerState::CreateLinearClamp(graphicsDevice);
		graphicsContext->SetSamplerState(0, samplerState);
	}
	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;
		
		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3(-0.8f, -0.8f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-0.8f,  0.8f, 0.0f), Vector2(0.0f, 1.0f),
			Vector3( 0.8f,  0.8f, 0.0f), Vector2(1.0f, 1.0f),
			Vector3( 0.8f, -0.8f, 0.0f), Vector2(1.0f, 0.0f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			VertexCombined::Declaration(), verticesCombo.data(), verticesCombo.size());

		effectPass = std::make_shared<EffectPass>(graphicsDevice, graphicsContext);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
			0, 1, 2,
			2, 3, 0
		};

		// Create index buffer
		indexBuffer = std::make_shared<ImmutableIndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size());
	}
//	{
//		// Draft:
//		struct ViewProjection
//		{
//			Matrix4x4 view;
//			Matrix4x4 projection;
//		};
//	
//		struct Material
//		{
//			Color ambient;
//			Color diffuse;
//			Color specular;
//		};
//
//		ViewProjection viewProjection;
//		Material material;
//		
//		// shared constant buffer
//		auto effectConstant = effectPass->GetConstant("World");
//		otherEffectPass->ResetConstant("World", effectConstant);
//		
//		// set value
//		effectPass->GetConstant("World")->SetValue(Float4x4::Identity);
//		effectPass->GetConstant("ViewProjection")->SetValue(viewProjection);
//		effectPass->GetConstant("Material")->SetValue(material);
//		
//		// shader reflection
//      auto shaderReflection = std::make_unique<ShaderReflection>(graphicsDevice);
//		auto parameterAnnotations = shaderReflection->GetParameters(effectPass);
//	}
	{
		for (auto & parameter: effectPass->Parameters()) {
			Log::Stream() << "EffectParameter: " << parameter.first;
		}
	}
}
//-----------------------------------------------------------------------
void CocoaTestGame::Update()
{
	//auto parameter = effectPass->Parameters("AmbientColor");
	//parameter->SetValue(Color{0.9f, 0.7f, 0.5f, 1.0f});
	
	static float value = 0.0f;
	value += 0.008f;
	if (value > 1.0f) {
		value = -1.0f;
	}
	
	Vector2 vec {
		std::abs(value),
		(1.0f + value) * 0.5f
	};
	
	auto parameter = effectPass->Parameters("TestStructure");
	parameter->SetValue(vec);
}
//-----------------------------------------------------------------------
void CocoaTestGame::Draw()
{
	graphicsContext->Clear(Color::CornflowerBlue);
	
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->GetIndexCount());
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
