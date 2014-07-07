//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MaidBeamGame.hpp"
#include <utility>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"
#include "Graphics/SpriteLine.hpp"
#include "2D/Renderer.hpp"
#include "2D/RenderCommand.hpp"
#include "2D/RenderQueue.hpp"
#include "2D/SkinnedMeshRenderer.hpp"
#include "2D/BeamRenderer.hpp"


namespace TestApp {

class EntityInterface {
public:
	EntityInterface(GameObject & gameObjectIn)
		: Transform(*gameObjectIn.Component<Transform2D>())
	{
		POMDOG_ASSERT(gameObjectIn.HasComponent<Transform2D>());
	}
	
	Transform2D & Transform;
};
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
	window->Title("TestApp - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(true);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		samplerPoint = SamplerState::CreateLinearWrap(graphicsDevice);
		
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
		texture = assets->Load<Texture2D>("pomdog.png");
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	{
		spriteRenderer = MakeUnique<SpriteRenderer>(graphicsContext, graphicsDevice, *assets);		
		fxaa = MakeUnique<FXAA>(gameHost);
		polygonBatch = MakeUnique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
	}
	{
		gameEditor = MakeUnique<SceneEditor::InGameEditor>(gameHost);
	}

	{
		mainCamera = gameWorld.CreateObject();
		mainCamera->AddComponent<Transform2D>();
		mainCamera->AddComponent<Camera2D>();
	}
	{
		maid = gameWorld.CreateObject();
		maid->AddComponent<Transform2D>();
		auto & renderer = maid->AddComponent(MakeUnique<SkinnedMeshRenderer>());
		renderer.Load(graphicsDevice, assets);
	}
	{
		lightningBeam = gameWorld.CreateObject();
		lightningBeam->AddComponent<Transform2D>();
		auto & renderer = lightningBeam->AddComponent(MakeUnique<BeamRenderer>());
		renderer.Load(graphicsDevice, assets);
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
			slider1 = std::make_shared<UI::Slider>(0.3, 1.48);
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
			toggleSwitch2->IsOn(true);
			stackPanel->AddChild(toggleSwitch2);
		}
		{
			toggleSwitch3 = std::make_shared<UI::ToggleSwitch>();
			toggleSwitch3->IsOn(false);
			stackPanel->AddChild(toggleSwitch3);
		}
		{
			toggleSwitch4 = std::make_shared<UI::ToggleSwitch>();
			toggleSwitch4->IsOn(false);
			stackPanel->AddChild(toggleSwitch4);
		}
	}
	
	clientSizeChangedConnection = window->ClientSizeChanged.Connect([this] {
		auto gameWindow = gameHost->Window();
		auto bounds = gameWindow->ClientBounds();
		
		renderTarget = std::make_shared<RenderTarget2D>(
			gameHost->GraphicsDevice(), bounds.Width, bounds.Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

		fxaa->ResetViewportSize(bounds);
		spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(bounds.Width, bounds.Height, 1.0f, 100.0f));
	});
}
//-----------------------------------------------------------------------
void MaidBeamGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		gameEditor->Update();
	}
	{
//		maidAnimationState->PlaybackRate(slider1->Value());
//		
//		if (toggleSwitch1->IsOn()) {
//			maidAnimationState->Stop();
//		}

		if (auto maidRenderer = maid->Component<SkinnedMeshRenderer>())
		{
			POMDOG_ASSERT(maidRenderer);
			maidRenderer->Update(*clock);
		}
	}
//	{
//		auto keyboard = gameHost->Keyboard();
//		
//		if (auto transform = maid->Component<Transform2D>())
//		{
//			Vector2 velocity = Vector2::Zero;
//
//			if (keyboard->State().IsKeyDown(Keys::A)) {
//				velocity.X -= 300.0f;
//			}
//			if (keyboard->State().IsKeyDown(Keys::D)) {
//				velocity.X += 300.0f;
//			}
//			if (keyboard->State().IsKeyDown(Keys::W)) {
//				velocity.Y += 50.0f;
//			}
//			if (keyboard->State().IsKeyDown(Keys::S)) {
//				velocity.Y -= 50.0f;
//			}
//			
//			transform->Position = transform->Position + (velocity * clock->FrameDuration().count());
//		}
//	}

	gameWorld.RemoveUnusedObjects();
}
//-----------------------------------------------------------------------
void MaidBeamGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
			
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			gameHost->Window()->ClientBounds().Width, gameHost->Window()->ClientBounds().Height, camera->Near, camera->Far);
		
		gameEditor->SetViewProjection(viewMatrix * projectionMatrix);
	}
	gameEditor->BeginDraw(*graphicsContext);

	graphicsContext->SetSamplerState(0, samplerPoint);
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
		
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			gameHost->Window()->ClientBounds().Width, gameHost->Window()->ClientBounds().Height, camera->Near, camera->Far);

		RenderQueue renderQueue;

		for (auto & gameObject: gameWorld.QueryComponents<Renderer, Transform2D>())
		{
			auto renderer = gameObject->Component<Renderer>();
			renderer->Visit(*gameObject, renderQueue, viewMatrix, projectionMatrix);
		}
		
		renderQueue.Sort();
		renderQueue.Enumerate([this](RenderCommand & command)
		{
			command.Execute(graphicsContext);
		});
		renderQueue.Clear();
	}

	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		fxaa->Draw(*graphicsContext, renderTarget);
	}

	gameEditor->EndDraw(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
