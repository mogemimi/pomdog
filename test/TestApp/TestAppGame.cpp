//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "TestAppGame.hpp"
#include <utility>
#include "SpriteRenderer.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
TestAppGame::TestAppGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	POMDOG_ASSERT(gameHostIn);
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
TestAppGame::~TestAppGame() = default;
//-----------------------------------------------------------------------
void TestAppGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("TestApp - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		auto sampler = SamplerState::CreatePointWrap(graphicsDevice);
		graphicsContext->SetSamplerState(0, sampler);

		texture = assets->Load<Texture2D>("pomdog.png");
	}
	
	spriteRenderer = std::unique_ptr<SpriteRenderer>(new SpriteRenderer(gameHost));
	
	auto gameObject = gameWorld.CreateObject();
	gameObject->AddComponent<CanvasItem>();
	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Sprite>();
}
//-----------------------------------------------------------------------
void TestAppGame::Update()
{
	static float value = 0.0f;
		value += 0.008f;
		if (value > 1.0f) {
			value = -1.0f;
		}

	for (auto gameObject: gameWorld.QueryComponents<CanvasItem, Transform2D>())
	{
		auto transform = gameObject->Component<Transform2D>();
		transform->Position.x += 1.0f;
		transform->Position.x = std::min(200.0f, transform->Position.x);
		transform->Scale.x = transform->Scale.y = 4.0f * (0.5f + (value * 0.5f));
		transform->Rotation = MathConstants<float>::Pi() * value;
		
		auto sprite = gameObject->Component<Sprite>();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, 16, 28);
	}
}
//-----------------------------------------------------------------------
void TestAppGame::Draw()
{
	graphicsContext->Clear(Color::CornflowerBlue);
	
	POMDOG_ASSERT(spriteRenderer);
	
	for (auto gameObject: gameWorld.QueryComponents<CanvasItem, Transform2D>())
	{
		auto canvasItem = gameObject->Component<CanvasItem>();
		if (!canvasItem->Visibile) {
			continue;
		}
		
		auto transform = gameObject->Component<Transform2D>();
		auto sprite = gameObject->Component<Sprite>();
		
		float layerDepth = 0.0f;
		spriteRenderer->Draw(texture, transform->Position, transform->Scale, transform->Rotation, sprite->Subrect, sprite->Origin, layerDepth);
	}
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
