//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MaidBeamGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include "Levels/GunShootingLevel.hpp"
#include <utility>
#include <random>

namespace TestApp {
//-----------------------------------------------------------------------
MaidBeamGame::MaidBeamGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
	, sandboxMode(false)
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
		fxaa = std::make_unique<FXAA>(graphicsDevice);
		auto bounds = window->ClientBounds();
		fxaa->SetViewport(bounds.Width, bounds.Height);
		renderer = std::make_unique<Renderer>(graphicsContext, graphicsDevice);
	}
	{
		editorCamera = gameWorld.CreateObject();
		editorCamera.AddComponent<Transform2D>();
		editorCamera.AddComponent<Camera2D>();
		
		level = std::make_unique<GunShootingLevel>(*gameHost, gameWorld);
	}
	{
		auto texture = assets->Load<Texture2D>("EditorResources/CameraIcon.png");
		for (size_t i = 0; i < 10; ++i) {
			cameraSprites.emplace_back(texture);
			cameraSprites.back().DrawOrder = 10000.0f;
		}
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
		auto stackPanel = std::make_shared<UI::StackPanel>(140, 170);
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
			toggleSwitch2->IsOn(sandboxMode);
			toggleSwitch2->OnContent("Sandbox On");
			toggleSwitch2->OffContent("Sandbox Off");
			cameraChangedConnection = toggleSwitch2->Toggled.Connect([this](bool isOn) {
				sandboxMode = isOn;
				scenePanel->IsEnabled(sandboxMode);
			});
			
			stackPanel->AddChild(toggleSwitch2);
		}
	}
	
	clientSizeChangedConnection = window->ClientSizeChanged.Connect([this](int width, int height) {
		graphicsContext->Viewport(Viewport{0, 0, width, height});
		graphicsContext->ScissorRectangle(Rectangle{0, 0, width, height});

		renderTarget = std::make_shared<RenderTarget2D>(
			gameHost->GraphicsDevice(), width, height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

		fxaa->SetViewport(width, height);
		Log::Info("ClientSizeChanged!");
	});
}
//-----------------------------------------------------------------------
void MaidBeamGame::Update()
{
	auto clock = gameHost->Clock();

	level->Update(*gameHost, gameWorld);

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

	gameWorld.Refresh();
	
	{
		gameEditor->Update();
		textBlock1->Text(StringFormat("Draw Calls: %d", renderer->DrawCallCount()));
	}
}
//-----------------------------------------------------------------------
void MaidBeamGame::DrawScene(Transform2D const& transform, Camera2D const& camera)
{
	auto clientBounds = gameHost->Window()->ClientBounds();
	
	Viewport viewport(
		clientBounds.Width * camera.ViewportX,
		clientBounds.Height * camera.ViewportY,
		clientBounds.Width * camera.ViewportWidth,
		clientBounds.Height * camera.ViewportHeight);
	
	auto viewMatrix = SandboxHelper::CreateViewMatrix(transform, camera);
	auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
		viewport.Width(), viewport.Height(), camera.Near, camera.Far);
	
	gameEditor->SetViewProjection(viewMatrix * projectionMatrix);
	renderer->ViewMatrix(viewMatrix);
	renderer->ProjectionMatrix(projectionMatrix);

	for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
	{
		auto renderable = gameObject.Component<Renderable>();
		renderable->Visit(gameObject, *renderer, viewMatrix, projectionMatrix);
	}

	graphicsContext->Viewport(viewport);
	
	if (sandboxMode)
	{
		size_t cameraIndex = 0;
		for (auto & gameObject: gameWorld.QueryComponents<Camera2D, Transform2D>())
		{
			if (cameraIndex >= cameraSprites.size()) {
				break;
			}
			if (editorCamera == gameObject) {
				continue;
			}
			cameraSprites[cameraIndex].Visit(gameObject, *renderer, viewMatrix, projectionMatrix);
			++cameraIndex;
		}

		gameEditor->BeginDraw(*graphicsContext);
	}

	renderer->Render(graphicsContext);
}
//-----------------------------------------------------------------------
void MaidBeamGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	{
		auto viewport = graphicsContext->Viewport();
		
		if (!sandboxMode)
		{
			bool cleared = false;
			
			for (auto & cameraObject: gameWorld.QueryComponents<Camera2D, Transform2D>())
			{
				auto camera = cameraObject.Component<Camera2D>();
				
				if (!camera->Enabled) {
					continue;
				}
				
				if (!cleared)
				{
					graphicsContext->Clear(camera->BackgroundColor);
					cleared = true;
				}
										
				auto transform = cameraObject.Component<Transform2D>();
				
				POMDOG_ASSERT(transform && camera);
				DrawScene(*transform, *camera);
			}
		}
		else
		{
			auto camera = editorCamera.Component<Camera2D>();
			auto transform = editorCamera.Component<Transform2D>();
			
			graphicsContext->Clear(camera->BackgroundColor);
			DrawScene(*transform, *camera);
		}
		
		graphicsContext->Viewport(viewport);
	}
	
	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		graphicsContext->Clear(Color::CornflowerBlue);
		fxaa->SetTexture(renderTarget);
		fxaa->Draw(*graphicsContext);
	}

	gameEditor->EndDraw(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
