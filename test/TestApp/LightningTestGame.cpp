//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "LightningTestGame.hpp"
#include <utility>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "SpriteFontLoader.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"

///@note for test
#include "ParticleEmitterShapeBox.hpp"
#include "ParticleEmitterShapeSector.hpp"
#include "ParticleParameterConstant.hpp"
#include "ParticleParameterCurve.hpp"
#include "ParticleParameterRandom.hpp"
#include "ParticleParameterRandomCurves.hpp"
#include "SpriteLine.hpp"


namespace TestApp {
//-----------------------------------------------------------------------
LightningTestGame::LightningTestGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	POMDOG_ASSERT(gameHostIn);
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
LightningTestGame::~LightningTestGame() = default;
//-----------------------------------------------------------------------
void LightningTestGame::Initialize()
{
	auto window = gameHost->Window();
	window->Title("TestApp - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(true);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		gameEditor = MakeUnique<SceneEditor::InGameEditor>(gameHost);
	}
	{
		//samplerPoint = SamplerState::CreatePointWrap(graphicsDevice);
		//graphicsContext->SetSamplerState(0, samplerPoint);
		
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
		texture = assets->Load<Texture2D>("Particles/lightning.png");

		blendStateAdditive = BlendState::CreateAdditive(graphicsDevice);
		blendStateNonPremultiplied = BlendState::CreateNonPremultiplied(graphicsDevice);
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}

	spriteBatch = MakeUnique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
	spriteRenderer = MakeUnique<SpriteRenderer>(graphicsContext, graphicsDevice, *assets);
	fxaa = MakeUnique<FXAA>(gameHost);

	rootNode = std::make_shared<HierarchyNode>();
	{
		auto gameObject = gameWorld.CreateObject();
		mainCamera = gameObject;

		gameObject->AddComponent<Transform2D>();
		gameObject->AddComponent<CanvasItem>();
		gameObject->AddComponent<Camera2D>();

		auto node = std::make_shared<HierarchyNode>(gameObject);
		rootNode->AddChild(node);
	}
	{
		auto gameObject = gameWorld.CreateObject();

		auto sprite = gameObject->AddComponent<Sprite>();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, texture->Width(), texture->Height());
	
		gameObject->AddComponent<Transform2D>();
		gameObject->AddComponent<CanvasItem>();
		
		auto transform = gameObject->Component<Transform2D>();
		transform->Position = {0, 0};
		transform->Scale = {2, 2};
		
		auto node = std::make_shared<HierarchyNode>(gameObject);
		rootNode->AddChild(node);
	}
	
	for (int i = 0; i < 10; ++i)
	{
		auto gameObject = gameWorld.CreateObject();
		gameObject->AddComponent<CanvasItem>();

		auto sprite = gameObject->AddComponent<Sprite>();
		auto transform = gameObject->AddComponent<Transform2D>();
		
		transform->Position = {i * 64 * 2.0f, 0};
		transform->Scale = {2.0f, 2.0f};
		transform->Rotation = (0.5f * i) * MathConstants<float>::PiOver4();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, texture->Width(), texture->Height());//Rectangle(0, 0, 16, 28);
		
		auto node = std::make_shared<HierarchyNode>(gameObject);
		rootNode->AddChild(node);
	}
	
	touchPoint = {0, -300};
	
	{
		scenePanel = std::make_shared<UI::ScenePanel>(window->ClientBounds().Width, window->ClientBounds().Height);
		scenePanel->drawOrder = 1.0f;
		scenePanel->cameraObject = mainCamera;
		gameEditor->AddView(scenePanel);
	}
	{
		auto stackPanel = std::make_shared<UI::StackPanel>(124, 170);
		stackPanel->drawOrder = 0.2f;
		stackPanel->Transform(Matrix3x2::CreateTranslation(Vector2{5, 10}));
		gameEditor->AddView(stackPanel);

		{
			auto navigator = std::make_shared<UI::DebugNavigator>(gameHost->Clock());
			navigator->drawOrder = 0.0f;
			stackPanel->AddChild(navigator);
		}
		{
			slider1 = std::make_shared<UI::Slider>(0, 100);
			slider1->drawOrder = 0.0f;
			slider1->Value(34);
			stackPanel->AddChild(slider1);
		}
		{
			slider2 = std::make_shared<UI::Slider>(0.1, 4.0);
			slider2->drawOrder = 0.0f;
			slider2->Value(1.2);
			stackPanel->AddChild(slider2);
		}
		{
			slider3 = std::make_shared<UI::Slider>(0.0f, 1.0f);
			slider3->drawOrder = 0.0f;
			slider3->Value(0.2f);
			stackPanel->AddChild(slider3);
		}
		{
			slider4 = std::make_shared<UI::Slider>(0.0f, 70.0f);
			slider4->drawOrder = 0.0f;
			slider4->Value(8.0f);
			stackPanel->AddChild(slider4);
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
void LightningTestGame::Update()
{
	auto clock = gameHost->Clock();
	{
		gameEditor->Update();
	}
	{
		beamSystem.emitter.InterpolationPoints = static_cast<std::uint16_t>(slider1->Value());
		beamSystem.emitter.StartThickness = slider2->Value();
		beamSystem.branching.BranchingRate = slider3->Value();
		beamSystem.emitter.SwayRange = std::uniform_real_distribution<float>(-slider4->Value(), slider4->Value());
	}
	{
		Transform2D transform;
		transform.Scale = {1.0f, 1.0f};
		transform.Rotation = 0.0f;
		transform.Position = Vector2::Zero;
		beamSystem.Update(clock->FrameDuration(), transform, touchPoint);
	}
}
//-----------------------------------------------------------------------
void LightningTestGame::DrawSprites()
{
	auto transform = mainCamera->Component<Transform2D>();
	auto camera = mainCamera->Component<Camera2D>();
		
	POMDOG_ASSERT(transform && camera);
	auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
	auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
		graphicsContext->Viewport().Width(), graphicsContext->Viewport().Height(), 0.1f, 1000.0f);

	gameEditor->SetViewProjection(viewMatrix * projectionMatrix);

	// NOTE: Changing blend state
	//graphicsContext->SetBlendState(blendStateAdditive);

	POMDOG_ASSERT(spriteRenderer);
	spriteRenderer->Begin(SpriteSortMode::Deferred, viewMatrix);
	{
		SpriteLine spriteLine;
		spriteLine.Texture = texture;
		spriteLine.HalfCircleSize = {8, 32};
		spriteLine.InverseThickness = 5.0f;
		spriteLine.StartRectangle = {0, 0, 32, 64};
		spriteLine.MiddleRectangle = {32, 0, 1, 64};
		spriteLine.EndRectangle = {33, 0, 31, 64};

		auto DrawBeam = [&](std::vector<Vector2> const& points, float lineThickness, Color const& color)
		{
			for (std::size_t i = 1; i < points.size(); ++i)
			{
				POMDOG_ASSERT(i > 0);
				auto & start = points[i - 1];
				auto & end = points[i];
				spriteLine.Draw(*spriteRenderer, start, end, lineThickness, color, 0);
			}
		};
		
		for (auto & beam: beamSystem.beams)
		{
			DrawBeam(beam.Points, beam.Thickness, beam.Color);
		}
	}
	spriteRenderer->End();
	graphicsContext->SetBlendState(blendStateNonPremultiplied);
}
//-----------------------------------------------------------------------
void LightningTestGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::Black);
	gameEditor->BeginDraw(*graphicsContext);

	//graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	
	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		fxaa->Draw(*graphicsContext, renderTarget);
	}

	gameEditor->EndDraw(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
