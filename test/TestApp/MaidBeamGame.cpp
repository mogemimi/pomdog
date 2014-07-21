//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MaidBeamGame.hpp"
#include <utility>
#include <random>
#include "Graphics/FXAA.hpp"
#include "Utilities/SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"
#include "2D/Animator.hpp"
#include "2D/BeamRenderable.hpp"
#include "2D/Behavior.hpp"
#include "2D/ScriptBehavior.hpp"
#include "2D/SkinnedMeshRenderable.hpp"
#include "2D/SpriteRenderable.hpp"
#include "Levels/GunShootingLevel.hpp"


namespace TestApp {
//-----------------------------------------------------------------------
MaidBeamGame::MaidBeamGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	POMDOG_ASSERT(gameHostIn);
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
MaidBeamGame::~MaidBeamGame() = default;
//-----------------------------------------------------------------------
void MaidBeamGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("MaidBeamGame - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(true);
	
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
		editorCamera = gameWorld.CreateObject();
		editorCamera.AddComponent<Transform2D>();
		editorCamera.AddComponent<Camera2D>();

		level = std::make_unique<GunShootingLevel>(*gameHost, gameWorld);
	}
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}
	{
		scenePanel = std::make_shared<UI::ScenePanel>(window->ClientBounds().Width, window->ClientBounds().Height);
		scenePanel->cameraObject = editorCamera;
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
			textBlock1 = std::make_shared<UI::TextBlock>();
			textBlock1->Text("Draw Calls: --");
			stackPanel->AddChild(textBlock1);
		}
		{
			slider1 = std::make_shared<UI::Slider>(0.3, 1.6);
			slider1->Value(1.48);
			stackPanel->AddChild(slider1);
		}
		{
			slider2 = std::make_shared<UI::Slider>(0.0, 1.0);
			slider2->Value(1.0);
			stackPanel->AddChild(slider2);
		}
		{
			toggleSwitch1 = std::make_shared<UI::ToggleSwitch>();
			toggleSwitch1->IsOn(true);
			toggleSwitch1->OnContent("Play");
			toggleSwitch1->OffContent("Stop");
			stackPanel->AddChild(toggleSwitch1);
		}
		{
			toggleSwitch2 = std::make_shared<UI::ToggleSwitch>();
			toggleSwitch2->IsOn(false);
			toggleSwitch2->OnContent("Editor Mode");
			toggleSwitch2->OffContent("Game Mode");
			cameraChangedConnection = toggleSwitch2->Toggled.Connect([this](bool isOn) {
				if (isOn) {
					for (auto & gameObject: gameWorld.QueryComponents<Camera2D>())
					{
						auto camera = gameObject.Component<Camera2D>();
						camera->Enabled = (editorCamera == gameObject);
					}
				}
				else {
					for (auto & gameObject: gameWorld.QueryComponents<Camera2D>())
					{
						auto camera = gameObject.Component<Camera2D>();
						camera->Enabled = (editorCamera != gameObject);
					}
				}
			});
			
			stackPanel->AddChild(toggleSwitch2);
		}
	}
	
	clientSizeChangedConnection = window->ClientSizeChanged.Connect([this] {
		auto gameWindow = gameHost->Window();
		auto bounds = gameWindow->ClientBounds();
		
		renderTarget = std::make_shared<RenderTarget2D>(
			gameHost->GraphicsDevice(), bounds.Width, bounds.Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

		fxaa->ResetViewportSize(bounds);
	});
}
//-----------------------------------------------------------------------
void MaidBeamGame::Update()
{
	auto clock = gameHost->Clock();

	level->Update(*gameHost);

	for (auto & gameObject: gameWorld.QueryComponents<Animator>())
	{
		auto animator = gameObject.Component<Animator>();
		
		POMDOG_ASSERT(animator);
		animator->Update(*clock);
	}

	for (auto & gameObject: gameWorld.QueryComponents<Behavior>())
	{
		auto behavior = gameObject.Component<Behavior>();
		
		POMDOG_ASSERT(behavior);
		behavior->Update(gameObject, clock->FrameDuration());
	}

	gameWorld.RemoveUnusedObjects();
	
	{
		gameEditor->Update();
		textBlock1->Text(StringFormat("Draw Calls: %d", renderer->DrawCallCount()));
	}
}
//-----------------------------------------------------------------------
void MaidBeamGame::Draw()
{
	auto cameras = gameWorld.QueryComponents<Transform2D, Camera2D>();
	for (auto & mainCamera: cameras)
	{
		auto transform = mainCamera.Component<Transform2D>();
		auto camera = mainCamera.Component<Camera2D>();
		
		if (!camera->Enabled) {
			continue;
		}
		
		POMDOG_ASSERT(transform && camera);
		auto clientBounds = gameHost->Window()->ClientBounds();
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			clientBounds.Width, clientBounds.Height, camera->Near, camera->Far);
		
		gameEditor->SetViewProjection(viewMatrix * projectionMatrix);
		renderer->ViewMatrix(viewMatrix);
		renderer->ProjectionMatrix(projectionMatrix);

		for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
		{
			auto renderable = gameObject.Component<Renderable>();
			renderable->Visit(gameObject, *renderer, viewMatrix, projectionMatrix);
		}
		
		break;
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
