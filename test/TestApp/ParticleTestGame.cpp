//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleTestGame.hpp"
#include <utility>
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"
#include "Rendering/Renderer.hpp"
#include "2D/Renderable.hpp"
#include "2D/ParticleRenderable.hpp"

namespace TestApp {
//-----------------------------------------------------------------------
ParticleTestGame::ParticleTestGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	POMDOG_ASSERT(gameHostIn);
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
ParticleTestGame::~ParticleTestGame() = default;
//-----------------------------------------------------------------------
void ParticleTestGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("ParticleTestGame - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	{
		fxaa = std::make_unique<FXAA>(gameHost);
		renderer = std::make_unique<Renderer>(graphicsContext, graphicsDevice, *assets);
	}
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}
	
	{
		mainCamera = gameWorld.CreateObject();
		mainCamera->AddComponent<Transform2D>();
		mainCamera->AddComponent<Camera2D>();
	}
	{
		particleObject = gameWorld.CreateObject();
		particleObject->AddComponent<Transform2D>();
		auto & renderable = particleObject->AddComponent(std::make_unique<ParticleRenderable>());
		renderable.Load(graphicsContext, graphicsDevice, *assets);
	}
	
	{
		scenePanel = std::make_shared<UI::ScenePanel>(window->ClientBounds().Width, window->ClientBounds().Height);
		scenePanel->cameraObject = mainCamera;
		gameEditor->AddView(scenePanel);
	}
	{
		auto stackPanel = std::make_shared<UI::StackPanel>(124, 170);
		stackPanel->Transform(Matrix3x2::CreateTranslation(Vector2{5, 10}));
		gameEditor->AddView(stackPanel);

		{
			auto navigator = std::make_shared<UI::DebugNavigator>(gameHost->Clock());
			stackPanel->AddChild(navigator);
		}
		{
			slider1 = std::make_shared<UI::Slider>(1, 512);
			slider1->Value(170.0);
			stackPanel->AddChild(slider1);
		}
		{
			slider2 = std::make_shared<UI::Slider>(-200.0, 200.0);
			slider2->Value(0.0);
			stackPanel->AddChild(slider2);
		}
	}
	{
		scenePanel->SceneTouch.Connect([this](Vector2 const& positionInView) {
			auto transform = mainCamera->Component<Transform2D>();
			auto camera = mainCamera->Component<Camera2D>();
		
			POMDOG_ASSERT(transform && camera);
			auto inverseViewMatrix3D = Matrix4x4::Invert(SandboxHelper::CreateViewMatrix(*transform, *camera));
			
			auto position = Vector3::Transform(Vector3(
				positionInView.X - graphicsContext->Viewport().Width() / 2,
				positionInView.Y - graphicsContext->Viewport().Height() / 2,
				0), inverseViewMatrix3D);

			touchPoint = Vector2{position.X, position.Y};
		});
	}
}
//-----------------------------------------------------------------------
void ParticleTestGame::Update()
{
	auto clock = gameHost->Clock();
	{
		gameEditor->Update();
	}
	
	if (auto transform = particleObject->Component<Transform2D>())
	{
		transform->Position = touchPoint;
		transform->Rotation = MathConstants<float>::PiOver2();
		transform->Scale = {1, 1};
	}

	if (auto particleRenderable = particleObject->Component<ParticleRenderable>())
	{
		auto & particleSystem = particleRenderable->particleSystem;
		particleSystem.emitter.EmissionRate = static_cast<std::uint16_t>(slider1->Value());
		particleSystem.emitter.GravityModifier = slider2->Value();
	}
		
//	for (auto & gameObject: gameWorld.QueryComponents<ParticleRenderable, Transform2D>())
//	{
//		auto particleRenderable = gameObject->Component<ParticleRenderable>();
//		particleRenderable->Update(*particleObject, *clock);
//	}
	if (auto particleRenderable = particleObject->Component<ParticleRenderable>())
	{
		particleRenderable->Update(*particleObject, *clock);
	}
}
//-----------------------------------------------------------------------
void ParticleTestGame::Draw()
{
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
			
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			gameHost->Window()->ClientBounds().Width, gameHost->Window()->ClientBounds().Height, camera->Near, camera->Far);
		
		gameEditor->SetViewProjection(viewMatrix * projectionMatrix);

		for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
		{
			auto renderable = gameObject->Component<Renderable>();
			renderable->Visit(*gameObject, *renderer, viewMatrix, projectionMatrix);
		}
	}

	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	gameEditor->BeginDraw(*graphicsContext);
	renderer->Render(graphicsContext);

	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		fxaa->Draw(*graphicsContext, renderTarget);
	}

	gameEditor->EndDraw(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
