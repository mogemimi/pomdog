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
	
	
	for (int i = 0; i < 10; ++i)
	{
		auto gameObject = gameWorld.CreateObject();
		gameObject->AddComponent<CanvasItem>();
		auto transform = gameObject->AddComponent<Transform2D>();
		gameObject->AddComponent<Sprite>();
		
		transform->Position.X = i * 28 - 200;
		transform->Position.Y = (i % 5) * 40;
	}
	
	playerID = gameWorld.QueryComponents<Transform2D, CanvasItem, Sprite>().front()->ID();
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
		transform->Position.X += 1.0f;
		if (transform->Position.X > 350.0f) {
			transform->Position.X = -140.0f;
		}
		transform->Scale.X = transform->Scale.Y = 2.0f;//(0.5f + (value * 0.5f));
		transform->Rotation = MathConstants<float>::Pi() * value;
		
		auto sprite = gameObject->Component<Sprite>();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, 16, 28);
	}
	
	auto mouse = gameHost->Mouse();
	
	if (auto transform = gameWorld.Component<Transform2D>(playerID))
	{
		if (mouse->State().LeftButton == ButtonState::Pressed)
		{
			transform->Position.X -= 5.0f;
		}
		if (mouse->State().RightButton == ButtonState::Pressed)
		{
			transform->Position.X += 5.0f;
		}
		
		transform->Rotation += float(mouse->State().ScrollWheel) * 0.1f;
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
