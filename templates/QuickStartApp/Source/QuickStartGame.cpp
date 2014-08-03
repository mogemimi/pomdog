//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "QuickStartGame.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
CocoaTestGame::CocoaTestGame(std::shared_ptr<GameHost> host)
	: gameHost(std::move(host))
{
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
void CocoaTestGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("Cocoa Test Game");
	window->AllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	
	auto assets = gameHost->AssetManager();

	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;
		
		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3(-0.8f, -0.8f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-0.8f,  0.8f, 0.0f), Vector2(0.0f, 1.0f),
			Vector3( 0.8f,  0.8f, 0.0f), Vector2(1.0f, 1.0f),
			Vector3( 0.8f, -0.8f, 0.0f), Vector2(1.0f, 0.0f),
		};
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			verticesCombo.data(), verticesCombo.size(),
			VertexCombined::Declaration().StrideBytes(), BufferUsage::Immutable);

		effectPass = assets->Load<EffectPass>("SimpleEffect");
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
		auto sampler = SamplerState::CreatePointClamp(graphicsDevice);
		graphicsContext->SetSamplerState(0, sampler);

		texture = assets->Load<Texture2D>("pomdog.png");
		
		if (texture) {
			Log::Stream()
				<< "PNG to Texture2D: OK" << "\n"
				<< "        Width: " << texture->Width() << "\n"
				<< "       Height: " << texture->Height() << "\n"
				<< "   LevelCount: " << texture->LevelCount() << "\n"
				<< "SurfaceFormat: " << static_cast<int>(texture->Format()) << "\n";
		}
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height);
	}
}
//-----------------------------------------------------------------------
void CocoaTestGame::Update()
{
	static float value = 0.0f;
	value += 0.008f;
	if (value > 1.0f) {
		value = -1.0f;
	}
	
	Vector2 vec {
		std::abs(value),
		(1.0f + value) * 0.5f
	};
	
	auto parameter = constantBuffers->Find("TestStructure");
	parameter->SetValue(vec);
	
	auto vector2 = parameter->GetValue<Vector2>();
}
//-----------------------------------------------------------------------
void CocoaTestGame::Draw()
{
	graphicsContext->Clear(Color::CornflowerBlue);
	
	graphicsContext->SetTexture(0, texture);
	graphicsContext->SetTexture(0);
	graphicsContext->SetTexture(0, texture);
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	graphicsContext->SetEffectPass(effectPass);
	graphicsContext->SetConstantBuffers(constantBuffers);
	graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
