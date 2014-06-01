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
#include "PrimitiveAxes.hpp"
#include "PrimitiveGrid.hpp"
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"

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
static Matrix3x3 CreateViewMatrix2D(Transform2D const& transform, Camera2D const& camera)
{
	return Matrix3x3::CreateTranslation({-transform.Position.X, -transform.Position.Y})*
		Matrix3x3::CreateRotationZ(-transform.Rotation) *
		Matrix3x3::CreateScale({camera.Zoom(), camera.Zoom(), 1});
}
//-----------------------------------------------------------------------
static Matrix4x4 CreateViewMatrix3D(Transform2D const& transform, Camera2D const& camera)
{
	return Matrix4x4::CreateTranslation({-transform.Position.X, -transform.Position.Y, 1.0f})*
		Matrix4x4::CreateRotationZ(-transform.Rotation) *
		Matrix4x4::CreateScale({camera.Zoom(), camera.Zoom(), 1});
}

}// unnamed namespace
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
	window->AllowPlayerResizing(false);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		//samplerPoint = SamplerState::CreatePointWrap(graphicsDevice);
		//graphicsContext->SetSamplerState(0, samplerPoint);
		
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
		texture = assets->Load<Texture2D>("Particles/lightning.png");
		texture = assets->Load<Texture2D>("pomdog.png");
		
		blendStateAdditive = BlendState::CreateAdditive(graphicsDevice);
		blendStateNonPremultiplied = BlendState::CreateNonPremultiplied(graphicsDevice);
	}
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	
	primitiveAxes = MakeUnique<PrimitiveAxes>(gameHost);
	primitiveGrid = MakeUnique<PrimitiveGrid>(gameHost);
	spriteBatch = MakeUnique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
	fxaa = MakeUnique<FXAA>(gameHost);
	
	{
		// NOTE: Create main camera:
		auto gameObject = gameWorld.CreateObject();
		mainCameraID = gameObject->ID();
		
		auto node = gameObject->AddComponent<Node2D>(gameObject);
		gameObject->AddComponent<CanvasItem>();
		gameObject->AddComponent<Camera2D>();
		//auto sprite = gameObject->AddComponent<Sprite>();
		//sprite->Origin = Vector2{0.5f, 0.5f};
		//sprite->Subrect = Rectangle(0, 0, texture->Width(), texture->Height());//Rectangle(0, 0, 16, 28);
		node->Transform().Scale = Vector2{2.5f, 2.5f};
	}
	
	{
		auto gameObject = gameWorld.CreateObject();
		rootObjectID = gameObject->ID();
		
		auto node = gameObject->AddComponent<Node2D>(gameObject);
		auto sprite = gameObject->AddComponent<Sprite>();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, texture->Width(), texture->Height());
	
		auto & transform = node->Transform();
		transform.Position = {0, 0};
		transform.Scale = {2, 2};
	}
	
	auto rootNode = gameWorld.Component<Node2D>(rootObjectID);
	
	for (int i = 0; i < 10; ++i)
	{
		auto gameObject = gameWorld.CreateObject();
		gameObject->AddComponent<CanvasItem>();
		auto node = gameObject->AddComponent<Node2D>(gameObject);
		auto sprite = gameObject->AddComponent<Sprite>();
		auto & transform = node->Transform();
		
		transform.Position.X = i * 64 * 2.0f;
		transform.Position.Y = 0;
		transform.Scale.X = transform.Scale.Y = 2.0f;
		transform.Rotation = (0.5f * i) * MathConstants<float>::PiOver4();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, texture->Width(), texture->Height());//Rectangle(0, 0, 16, 28);
		
		rootNode->AddChild(gameObject);
	}
	
	{
		line.Point1 = Vector2{0, 0};
		line.Point2 = Vector2{100, 0};
		line.Thickness = 1.0f;
	}
}
//-----------------------------------------------------------------------
void LightningTestGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		auto node = gameWorld.Component<Node2D>(mainCameraID);
		auto camera = gameWorld.Component<Camera2D>(mainCameraID);
		
		if (node && camera)
		{
			cameraView.Input(mouse->State(), *clock, graphicsContext->Viewport().Bounds, node->Transform(), *camera);
		}
	}
	{
		static auto duration = DurationSeconds(0);
		
		if (clock->TotalGameTime() - duration > DurationSeconds(0.2)) {
			gameHost->Window()->Title(StringFormat("%f fps", clock->FrameRate()));
			duration = clock->TotalGameTime();
		}
	}
	{
		if (mouse->State().RightButton == ButtonState::Pressed)
		{
			auto node = gameWorld.Component<Node2D>(mainCameraID);
			auto camera = gameWorld.Component<Camera2D>(mainCameraID);
		
			POMDOG_ASSERT(node && camera);
		
			auto inverseViewMatrix3D = Matrix4x4::Invert(CreateViewMatrix3D(node->Transform(), *camera));
			
			auto position = Vector3::Transform(Vector3(
					mouse->State().Position.X - graphicsContext->Viewport().Width()/2,
					mouse->State().Position.Y - graphicsContext->Viewport().Height()/2,
					0), inverseViewMatrix3D);

			line.Point2 = Vector2{position.X, position.Y};
		}
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
void LightningTestGame::DrawSprites()
{
	auto camera = gameWorld.Component<Camera2D>(mainCameraID);
	auto nodeCamera = gameWorld.Component<Node2D>(mainCameraID);
	
	auto viewMatrix3D = CreateViewMatrix3D(nodeCamera->Transform(), *camera);
	auto projectionMatrix3D = Matrix4x4::CreateOrthographicLH(800.0f, 480.0f, 0.1f, 1000.0f);
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, viewMatrix3D * projectionMatrix3D);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, viewMatrix3D * projectionMatrix3D);
	
	auto viewMatrix2D = CreateViewMatrix2D(nodeCamera->Transform(), *camera);
	
	// NOTE: Changing blend state
	//graphicsContext->SetBlendState(blendStateAdditive);

	POMDOG_ASSERT(spriteBatch);
	spriteBatch->Begin(viewMatrix2D);
	{
		float layerDepth = 0;
		
		//auto lineLength = Vector2::Distance(line.Point2, line.Point1);
		
		Vector2 const HalfCircleSize = {8, 32};
		
		auto tangent = line.Point2 - line.Point1;
		auto rotation = std::atan2(tangent.Y, tangent.X);
		
		//spriteBatch->Draw(texture, line.Point1 + Vector2{0.5f, 0}, Rectangle{16, 0, 1, 32}, Color::White,
		//	rotation, {0.0f, 0.5f}, Vector2{lineLength, 1}, layerDepth);
		
		spriteBatch->Draw(texture, line.Point1, Rectangle{0, 0, 15, 32}, Color::Black,
			rotation, {1.0f, 1.0f}, Vector2{1, 1}, layerDepth);
		spriteBatch->Draw(texture, line.Point2, Rectangle{17, 0, 15, 32}, Color::Black,
			rotation, {0.0f, 0.5f}, Vector2{1, 1}, layerDepth);
	}
	spriteBatch->End();
	graphicsContext->SetBlendState(blendStateNonPremultiplied);
}
//-----------------------------------------------------------------------
void LightningTestGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	
	//graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	
	if (enableFxaa) {
		fxaa->Draw(*graphicsContext, renderTarget);
	}
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
