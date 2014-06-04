//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ParticleTestGame.hpp"
#include <utility>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "PrimitiveAxes.hpp"
#include "PrimitiveGrid.hpp"
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"

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
	
	//emitter.Shape = MakeUnique<ParticleEmitterShapeSector>(MathConstants<float>::PiOver4());
	emitter.Shape = MakeUnique<ParticleEmitterShapeBox>(0, 100);
	
	emitter.StartSpeed = MakeUnique<ParticleParameterRandom<float>>(40.0f, 128.0f);
	//emitter.StartSpeed = MakeUnique<ParticleParameterConstant<float>>(-128.0f);
	
//	emitter.StartSpeed = MakeUnique<ParticleParameterCurve<float>>(
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
	
	emitter.StartColor = MakeUnique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = MakeUnique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = MakeUnique<ParticleParameterRandom<Color>>(Color::Black, Color::White);
	
	//emitter.ColorOverLifetime = MakeUnique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.ColorOverLifetime = MakeUnique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
	emitter.ColorOverLifetime = MakeUnique<ParticleParameterCurve<Color>>(
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
	
	//emitter.StartRotation = MakeUnique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.StartRotation = MakeUnique<ParticleParameterRandom<Radian<float>>>(
		0, MathConstants<float>::TwoPi());
	
	//emitter.RotationOverLifetime = MakeUnique<ParticleParameterConstant<Radian<float>>>(0);
	emitter.RotationOverLifetime = MakeUnique<ParticleParameterRandom<Radian<float>>>(
		-MathConstants<float>::PiOver4(), MathConstants<float>::PiOver4());
	
	//emitter.StartSize = MakeUnique<ParticleParameterConstant<float>>(1.0f);
	emitter.StartSize = MakeUnique<ParticleParameterRandom<float>>(0.8f, 1.2f);
	
	//emitter.SizeOverLifetime = MakeUnique<ParticleParameterConstant<float>>(1.0f);
	emitter.SizeOverLifetime = MakeUnique<ParticleParameterCurve<float>>(
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
	
	primitiveAxes = MakeUnique<PrimitiveAxes>(gameHost, editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ);
	primitiveGrid = MakeUnique<PrimitiveGrid>(gameHost, editorColorScheme.GuideLine, editorColorScheme.Grid);
	spriteBatch = MakeUnique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
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
		particleSystem.emitter = CreateEmitterFireBlock();
	}
}
//-----------------------------------------------------------------------
void ParticleTestGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
		
		if (transform && camera)
		{
			cameraView.Input(mouse->State(), *clock, graphicsContext->Viewport().Bounds, *transform, *camera);
		}
	}
	{
		static auto duration = DurationSeconds(0);
		
		if (clock->TotalGameTime() - duration > DurationSeconds(0.2)) {
			gameHost->Window()->Title(StringFormat("%f fps", clock->FrameRate()));
			duration = clock->TotalGameTime();
		}
	}
//	{
//		static Transform2D emitterTranform;
//	
//		if (mouse->State().RightButton == ButtonState::Pressed)
//		{
//			auto node = gameWorld.Component<Node2D>(mainCameraID);
//			auto camera = gameWorld.Component<Camera2D>(mainCameraID);
//		
//			POMDOG_ASSERT(node && camera);
//		
//			auto inverseViewMatrix3D = Matrix4x4::Invert(SandboxHelper::CreateViewMatrix3D(node->Transform(), *camera));
//			
//			auto position = Vector3::Transform(Vector3(
//					mouse->State().Position.X - graphicsContext->Viewport().Width()/2,
//					mouse->State().Position.Y - graphicsContext->Viewport().Height()/2,
//					0), inverseViewMatrix3D);
//
//			emitterTranform.Position = Vector2{position.X, position.Y};
//			emitterTranform.Rotation = MathConstants<float>::PiOver2();
//			particleSystem.ResetEmission();
//		}
//		
//		particleSystem.Update(clock->FrameDuration(), emitterTranform);
//	}
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
	auto viewMatrix3D = SandboxHelper::CreateViewMatrix3D(*transform, *camera);
	auto projectionMatrix3D = Matrix4x4::CreateOrthographicLH(800.0f, 480.0f, 0.1f, 1000.0f);
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, viewMatrix3D * projectionMatrix3D);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, viewMatrix3D * projectionMatrix3D);
	
	auto viewMatrix2D = SandboxHelper::CreateViewMatrix2D(*transform, *camera);
	
	// NOTE: Changing blend state
	//graphicsContext->SetBlendState(blendStateAdditive);

	POMDOG_ASSERT(spriteBatch);
	spriteBatch->Begin(viewMatrix2D);
	{
		for (auto & particle: particleSystem.particles)
		{
			spriteBatch->Draw(texture, particle.Position, Rectangle(0, 0, texture->Width(), texture->Height()),
				particle.Color, particle.Rotation, {0.5f, 0.5f}, particle.Size, 0);
		}
	}
	spriteBatch->End();
	graphicsContext->SetBlendState(blendStateNonPremultiplied);
}
//-----------------------------------------------------------------------
void ParticleTestGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(editorColorScheme.Background);
	
	//graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	
	if (enableFxaa) {
		fxaa->Draw(*graphicsContext, renderTarget);
	}
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
