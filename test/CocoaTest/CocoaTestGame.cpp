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
		BlendDescription description;
		auto blendState = std::make_shared<BlendState>(graphicsDevice, description);
		graphicsContext->SetBlendState(blendState);
	}
	{
		DepthStencilDescription description;
		auto depthStencilState = std::make_shared<DepthStencilState>(graphicsDevice, description);
		graphicsContext->SetDepthStencilState(depthStencilState);
	}
	{
		RasterizerDescription description;
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
			Vector3, Vector2, Vector3
		>;
		std::array<VertexCombined, 4> verticesCombo = {
			Vector3(+1.f, -1.f, 0.f), Vector2(1.f, 0.f), Vector3(1.f, 0.f, 0.f),
			Vector3(+1.f, +1.f, 0.f), Vector2(1.f, 1.f), Vector3(0.f, 1.f, 0.f),
			Vector3(-1.f, +1.f, 0.f), Vector2(0.f, 1.f), Vector3(0.f, 0.f, 1.f),
			Vector3(-1.f, -1.f, 0.f), Vector2(0.f, 0.f), Vector3(0.f, 0.f, 0.f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			VertexCombined::Declaration(), verticesCombo.data(), verticesCombo.size());
	}
	{
		effectPass = std::make_shared<EffectPass>(graphicsDevice);
	}
}
//-----------------------------------------------------------------------
void CocoaTestGame::Update()
{
}
//-----------------------------------------------------------------------
void CocoaTestGame::Draw()
{
	auto color = Pomdog::Color::CornflowerBlue;
	graphicsContext->Clear(ClearOptions::RenderTarget|ClearOptions::DepthBuffer, color, 0.0f, 0);
	
	//effectPass->Apply();
	//graphicsContext->Draw(PrimitiveTopology::TriangleStrip);
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
