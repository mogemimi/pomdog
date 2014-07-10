//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleTestGame.hpp"
#include <utility>
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"

///@note for test
#include "ParticleEmitterShapeBox.hpp"
#include "ParticleEmitterShapeSector.hpp"
#include "ParticleParameterConstant.hpp"
#include "ParticleParameterCurve.hpp"
#include "ParticleParameterRandom.hpp"
#include "ParticleParameterRandomCurves.hpp"

namespace TestApp {
namespace {
//-----------------------------------------------------------------------
static ParticleEmitter CreateEmitterFireBlock()
{
	ParticleEmitter emitter;

	emitter.MaxParticles = 1024;
	emitter.EmissionRate = 128*2;
	emitter.Duration = DurationSeconds{0.1};
	//emitter.Looping = false;
	emitter.StartLifetime = 1.8f;
	//emitter.GravityModifier = 100.0f;
	
	//emitter.Shape = std::make_unique<ParticleEmitterShapeSector>(MathConstants<float>::PiOver4());
	emitter.Shape = std::make_unique<ParticleEmitterShapeBox>(0, 100);
	
	emitter.StartSpeed = std::make_unique<ParticleParameterRandom<float>>(40.0f, 128.0f);
	//emitter.StartSpeed = std::make_unique<ParticleParameterConstant<float>>(-128.0f);
	
//	emitter.StartSpeed = std::make_unique<ParticleParameterCurve<float>>(
//		std::initializer_list<ParticleCurveKey<float>>{
//			{0.00f, 0.0f},
//			{0.10f, -0.5f},
//			{0.15f, -1.0f},
//			{0.20f, -0.5f},
//			{0.40f, 0.0f},
//			{0.50f, 0.5f},
//			{0.70f, 1.0f},
//			{0.80f, 0.5f},
//			{1.00f, 0.0f},
//		});
	
	emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::Black, Color::White);
	
	//emitter.ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
	emitter.ColorOverLifetime = std::make_unique<ParticleParameterCurve<Color>>(
		std::initializer_list<ParticleCurveKey<Color>>{
			{0.00f, Color{255, 255, 255, 0}},
			{0.02f, Color{255, 255, 255, 10}},
			{0.09f, Color{255, 250, 180, 100}},
			{0.15f, Color{255, 200, 180, 130}},
			{0.19f, Color{200, 130, 60, 255}},
			{0.24f, Color{190, 50, 10, 80}},
			{0.32f, Color{80, 24, 2, 20}},
			{1.00f, Color{0, 0, 0, 0}},
		});
	
	//emitter.StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		0, MathConstants<float>::TwoPi());
	
	//emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		-MathConstants<float>::PiOver4(), MathConstants<float>::PiOver4());
	
	//emitter.StartSize = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	emitter.StartSize = std::make_unique<ParticleParameterRandom<float>>(0.8f, 1.2f);
	
	//emitter.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	emitter.SizeOverLifetime = std::make_unique<ParticleParameterCurve<float>>(
		std::initializer_list<ParticleCurveKey<float>>{
			{0.00f, 0.0f},
			{0.03f, 0.5f},
			{0.10f, 0.8f},
			{0.15f, 1.0f},
			{0.60f, 0.8f},
			{1.00f, 0.0f},
		});

	return std::move(emitter);
}

}// unnamed namespace
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
	window->Title("TestApp - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		//samplerPoint = SamplerState::CreatePointWrap(graphicsDevice);
		//graphicsContext->SetSamplerState(0, samplerPoint);
		
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
		texture = assets->Load<Texture2D>("Particles/smoke.png");
		
		blendStateAdditive = BlendState::CreateAdditive(graphicsDevice);
		blendStateNonPremultiplied = BlendState::CreateNonPremultiplied(graphicsDevice);
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	{
		spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
		spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice, *assets);
		fxaa = std::make_unique<FXAA>(gameHost);
	}
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}
	
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
		particleSystem.emitter = CreateEmitterFireBlock();
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
	auto mouse = gameHost->Mouse();
	{
		gameEditor->Update();
	}
	{
		particleSystem.emitter.EmissionRate = static_cast<std::uint16_t>(slider1->Value());
		particleSystem.emitter.GravityModifier = slider2->Value();
	}
	{
		Transform2D transform;
		transform.Position = touchPoint;
		transform.Rotation = MathConstants<float>::PiOver2();
		transform.Scale = {1, 1};
		particleSystem.Update(clock->FrameDuration(), transform);
	}
	{
		static bool isPaused = false;
		static DurationSeconds time = DurationSeconds(0);
		
		if (clock->TotalGameTime() - time > DurationSeconds(0.2)) {
			if (mouse->State().LeftButton == ButtonState::Pressed) {
				time = clock->TotalGameTime();
				isPaused = !isPaused;
			}
		}
		
		if (isPaused) {
			return;
		}
	}
}
//-----------------------------------------------------------------------
void ParticleTestGame::DrawSprites()
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
		for (auto & particle: particleSystem.particles)
		{
			spriteRenderer->Draw(texture, Matrix3x2::Identity, particle.Position, Rectangle(0, 0, texture->Width(), texture->Height()),
				particle.Color, particle.Rotation, {0.5f, 0.5f}, particle.Size, 0);
		}
	}
	spriteRenderer->End();
	graphicsContext->SetBlendState(blendStateNonPremultiplied);
}
//-----------------------------------------------------------------------
void ParticleTestGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
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
