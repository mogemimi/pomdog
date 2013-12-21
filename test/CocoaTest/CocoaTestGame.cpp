//
//  Copyright (C) 2013 mogemimi.
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
		auto blendState = BlendState::CreateAdditive(graphicsDevice);
		//BlendDescription description;
		//auto blendState = std::make_shared<BlendState>(graphicsDevice, description);
		graphicsContext->SetBlendState(blendState);
	}
	{
		//auto depthStencilState = DepthStencilState::CreateReadOnlyDepth(graphicsDevice);
		DepthStencilDescription description;
		auto depthStencilState = std::make_shared<DepthStencilState>(graphicsDevice, description);
		graphicsContext->SetDepthStencilState(depthStencilState);
	}
	{
		//auto rasterizerState = RasterizerState::CreateCullCounterClockwise(graphicsDevice);
		RasterizerDescription description;
		description.CullMode = CullMode::None;
		auto rasterizerState = std::make_shared<RasterizerState>(graphicsDevice, description);
		graphicsContext->SetRasterizerState(rasterizerState);
	}
	{
		SamplerDescription description;
		auto samplerState = std::make_shared<SamplerState>(graphicsDevice, description);
		graphicsContext->SetSamplerState(0, samplerState);
	}
	{
		using VertexCombined = CustomVertex<
			Vector3, Vector2//, Vector3
		>;
		std::array<VertexCombined, 4> verticesCombo = {
			Vector3(+1.f, -1.f, 0.f), Vector2(1.f, 0.f), //Vector3(1.f, 0.f, 0.f),
			Vector3(+1.f, +1.f, 0.f), Vector2(1.f, 1.f), //Vector3(0.f, 1.f, 0.f),
			Vector3(-1.f, +1.f, 0.f), Vector2(0.f, 1.f), //Vector3(0.f, 0.f, 1.f),
			Vector3(-1.f, -1.f, 0.f), Vector2(0.f, 0.f), //Vector3(0.f, 0.f, 0.f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			VertexCombined::Declaration(), verticesCombo.data(), verticesCombo.size());

		effectPass = std::make_shared<EffectPass>(graphicsDevice, graphicsContext);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	
//		std::vector<VertexBufferBinding> vertexBinding {
//			{ VertexCombined::Declaration(), 0, 0 },
//			//{ VertexCombined::Declaration(), 0, 1 }
//		};
//		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass, vertexBinding);
	}
}
//-----------------------------------------------------------------------
void CocoaTestGame::Update()
{
}
//-----------------------------------------------------------------------
void CocoaTestGame::Draw()
{
	graphicsContext->Clear(Color::CornflowerBlue);
	
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->Draw(PrimitiveTopology::TriangleStrip);
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
