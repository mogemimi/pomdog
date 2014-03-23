//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "TestAppGame.hpp"
#include <utility>
#include "PrimitiveAxes.hpp"
#include "PrimitiveGrid.hpp"
#include "SpriteRenderer.hpp"

namespace TestApp {
namespace {

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
	{
		renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
			window->ClientBounds().Width, window->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
	}
	{
		using VertexCombined = CustomVertex<Vector3, Vector2>;
		
		std::array<VertexCombined, 4> const verticesCombo = {
			Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 0.0f),
			Vector3(-1.0f,  1.0f, 0.0f), Vector2(0.0f, 1.0f),
			Vector3( 1.0f,  1.0f, 0.0f), Vector2(1.0f, 1.0f),
			Vector3( 1.0f, -1.0f, 0.0f), Vector2(1.0f, 0.0f),
		};
		vertexBuffer = std::make_shared<ImmutableVertexBuffer>(graphicsDevice,
			VertexCombined::Declaration(), verticesCombo.data(), verticesCombo.size());

		effectPass = assets->Load<EffectPass>("SimpleEffect");
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
	{
		std::array<std::uint16_t, 6> const indices = {
			0, 1, 2,
			2, 3, 0
		};

		// Create index buffer
		indexBuffer = std::make_shared<ImmutableIndexBuffer>(graphicsDevice,
			IndexElementSize::SixteenBits, indices.data(), indices.size());
	}
	{
		for (auto & parameter: effectPass->Parameters()) {
			Log::Stream() << "EffectParameter: " << parameter.first;
		}
		
		auto effectReflection = std::make_shared<EffectReflection>(graphicsDevice, effectPass);
	
		auto stream = Log::Stream();
		for (auto & description: effectReflection->GetConstantBuffers()) {
			stream << "-----------------------" << "\n";
			stream << "     Name: " << description.Name << "\n";
			stream << " ByteSize: " << description.ByteSize << "\n";
			stream << "Variables: " << description.Variables.size() << "\n";
		}
	}
	
	primitiveAxes = std::unique_ptr<PrimitiveAxes>(new PrimitiveAxes(gameHost));
	primitiveGrid = std::unique_ptr<PrimitiveGrid>(new PrimitiveGrid(gameHost));
	spriteRenderer = std::unique_ptr<SpriteRenderer>(new SpriteRenderer(gameHost));
	
	
	{
		// NOTE: Create main camera:
		auto gameObject = gameWorld.CreateObject();;
		mainCameraID = gameObject->ID();
		
		auto transform = gameObject->AddComponent<Transform2D>();
		gameObject->AddComponent<CanvasItem>();
		auto sprite = gameObject->AddComponent<Sprite>();
		gameObject->AddComponent<Camera2D>();
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, 16, 28);
		transform->Scale = Vector2{2.5f, 2.5f};
	}
	
	for (int i = 0; i < 10; ++i)
	{
		auto gameObject = gameWorld.CreateObject();
		gameObject->AddComponent<CanvasItem>();
		auto transform = gameObject->AddComponent<Transform2D>();
		auto sprite = gameObject->AddComponent<Sprite>();
		
		transform->Position.X = i * 28 - 200;
		transform->Position.Y = (i % 5) * 40;
		sprite->Origin = Vector2{0.5f, 0.5f};
		sprite->Subrect = Rectangle(0, 0, 16, 28);
	}
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
		if (gameObject->ID() == mainCameraID) {
			continue;
		}
	
		auto transform = gameObject->Component<Transform2D>();
		transform->Position.X += 1.0f;
		if (transform->Position.X > 350.0f) {
			transform->Position.X = -140.0f;
		}
		transform->Scale.X = transform->Scale.Y = 2.0f;//(0.5f + (value * 0.5f));
		transform->Rotation = MathConstants<float>::Pi() * value;
	}
	
	{
		auto mouse = gameHost->Mouse();
		auto transform = gameWorld.Component<Transform2D>(mainCameraID);
		auto camera = gameWorld.Component<Camera2D>(mainCameraID);
		
		if (transform && camera)
		{
			cameraView.Input(mouse->State(), graphicsContext->GetViewport().Bounds, *transform, *camera);
		}
	}
}
//-----------------------------------------------------------------------
void TestAppGame::DrawSprites()
{
	auto camera = gameWorld.Component<Camera2D>(mainCameraID);
	auto transform = gameWorld.Component<Transform2D>(mainCameraID);
	
	auto vierMatrix3D = CreateViewMatrix3D(*transform, *camera);;
	auto projectionMatrix3D = Matrix4x4::CreateOrthographicLH(800.0f, 480.0f, 0.1f, 1000.0f);
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, vierMatrix3D * projectionMatrix3D);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, vierMatrix3D * projectionMatrix3D);
	
	auto viewMatrix2D = CreateViewMatrix2D(*transform, *camera);
	
	POMDOG_ASSERT(spriteRenderer);
	spriteRenderer->Begin(viewMatrix2D);
	
	for (auto gameObject: gameWorld.QueryComponents<CanvasItem, Transform2D, Sprite>())
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
	
	spriteRenderer->End();
}
//-----------------------------------------------------------------------
void TestAppGame::Draw()
{
	graphicsContext->SetRenderTarget(renderTarget);
	graphicsContext->Clear(Color::CornflowerBlue);
	
	DrawSprites();
	
	graphicsContext->SetRenderTarget();
	graphicsContext->Clear(Color::CornflowerBlue);
	
	graphicsContext->SetTexture(0, renderTarget);
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	effectPass->Apply();
	graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList, indexBuffer, indexBuffer->IndexCount());
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
