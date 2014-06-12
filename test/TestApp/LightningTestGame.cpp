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
class SpriteLine {
public:
	std::shared_ptr<Texture2D> Texture;
	Rectangle StartRectangle;
	Rectangle MiddleRectangle;
	Rectangle EndRectangle;
	Vector2 HalfCircleSize;
	float InverseThickness;
};
//-----------------------------------------------------------------------
static void DrawSpriteLine(SpriteBatch & spriteBatch, SpriteLine const& spriteLine,
	Vector2 const& point1, Vector2 const& point2, float lineThickness,
	Color const& color, float layerDepth)
{
	auto lineLength = Vector2::Distance(point2, point1);
	
	POMDOG_ASSERT(spriteLine.InverseThickness > 0);
	auto thicknessScale = lineThickness / spriteLine.InverseThickness;
	
	auto tangent = point2 - point1;
	auto rotation = std::atan2(tangent.Y, tangent.X);
	
	POMDOG_ASSERT(spriteLine.Texture);

	spriteBatch.Draw(spriteLine.Texture, point1, spriteLine.MiddleRectangle,
		color, rotation, {0.0f, 0.5f}, Vector2{lineLength, thicknessScale}, layerDepth);
	spriteBatch.Draw(spriteLine.Texture, point1, spriteLine.StartRectangle,
		color, rotation, {1.0f, 0.5f}, thicknessScale, layerDepth);
	spriteBatch.Draw(spriteLine.Texture, point2, spriteLine.EndRectangle,
		color, rotation, {0.0f, 0.5f}, thicknessScale, layerDepth);
}
//-----------------------------------------------------------------------
class SpriteBatchDrawingContext final: public UI::DrawingContext {
private:
	SpriteBatch & spriteBatch;
	std::shared_ptr<Texture2D> texture;
	
public:
	explicit SpriteBatchDrawingContext(SpriteBatch & spriteBatchIn, std::shared_ptr<Texture2D> const& textureIn)
		: spriteBatch(spriteBatchIn)
		, texture(textureIn)
	{
	}
	
	void DrawRectangle(Color const& color, Rectangle const& rectangle)
	{
		spriteBatch.Draw(texture, Vector2(rectangle.X, -rectangle.Y),
			Rectangle{0, 0, 1, 1}, color, 0, {0.0f, 1.0f}, Vector2(rectangle.Width, rectangle.Height), 0.0f);
	}
	
	void DrawLine(Color const& color, float penSize, Point2D const& point1, Point2D const& point2)
	{
	}
};

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
	window->AllowPlayerResizing(true);
//	{
//		auto bounds = window->ClientBounds();
//		bounds.Width = 1280;
//		bounds.Height = 720;
//		window->ClientBounds(bounds);
//	}
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
		//samplerPoint = SamplerState::CreatePointWrap(graphicsDevice);
		//graphicsContext->SetSamplerState(0, samplerPoint);
		
		auto blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
		graphicsContext->SetBlendState(blendState);
		texture = assets->Load<Texture2D>("Particles/lightning.png");
		pomdogTexture = assets->Load<Texture2D>("pomdog.png");
		
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
	backgroundPlane = MakeUnique<SceneEditor::GradientPlane>(gameHost);
	
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
		auto node = std::make_shared<UI::ScenePanel>(window->ClientBounds().Width, window->ClientBounds().Height);
		node->bounds = {0, 0, window->ClientBounds().Width, window->ClientBounds().Height};
		node->drawOrder = 1.0f;
		node->cameraObject = mainCamera;
	
		scenePanel = node;
		hierarchy.AddChild(std::move(node));
	}
	{
		auto slider = std::make_shared<UI::Slider>(0, 100);
		slider->drawOrder = 0.0f;
		slider->RenderTransform.Position = Vector2{35, 40};
		slider->Value(34);
	
		slider1 = slider;
		hierarchy.AddChild(std::move(slider));
	}
	{
		auto slider = std::make_shared<UI::Slider>(0.1, 4.0);
		slider->drawOrder = 0.0f;
		slider->RenderTransform.Position = Vector2{35, 65};
		slider->Value(1.2);
	
		slider2 = slider;
		hierarchy.AddChild(std::move(slider));
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
		
		renderTarget = std::make_shared<RenderTarget2D>(
			gameHost->GraphicsDevice(),
			gameWindow->ClientBounds().Width, gameWindow->ClientBounds().Height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
		
		auto viewport = graphicsContext->Viewport();
		fxaa->ResetViewportSize(viewport.Bounds);
		
		scenePanel->bounds.Width = viewport.Bounds.Width;
		scenePanel->bounds.Height = viewport.Bounds.Height;
	});
}
//-----------------------------------------------------------------------
void LightningTestGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		hierarchy.Touch(mouse->State());
		hierarchy.UpdateAnimation(clock->FrameDuration());
	}
	{
		beamSystem.emitter.InterpolationPoints = static_cast<std::uint16_t>(slider1->Value());
		beamSystem.emitter.StartThickness = slider2->Value();
	}
	{
		static auto duration = DurationSeconds(0);
		
		if (clock->TotalGameTime() - duration > DurationSeconds(0.2)) {
			gameHost->Window()->Title(StringFormat("%f fps", clock->FrameRate()));
			duration = clock->TotalGameTime();
			//Log::Info(StringFormat("Mouse: %d, %d", mouse->State().Position.X, mouse->State().Position.Y));
		}
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
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, viewMatrix * projectionMatrix);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, viewMatrix * projectionMatrix);
	
	// NOTE: Changing blend state
	//graphicsContext->SetBlendState(blendStateAdditive);

	POMDOG_ASSERT(spriteBatch);
	spriteBatch->Begin(viewMatrix);
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
				DrawSpriteLine(*spriteBatch, spriteLine, start, end, lineThickness, color, 0);
			}
		};
		
		for (auto & beam: beamSystem.beams)
		{
			DrawBeam(beam.Points, beam.Thickness, beam.Color);
		}
	}
	spriteBatch->End();
	graphicsContext->SetBlendState(blendStateNonPremultiplied);
}
//-----------------------------------------------------------------------
void LightningTestGame::DrawGUI()
{
	POMDOG_ASSERT(spriteBatch);
	auto viewportWidth = graphicsContext->Viewport().Bounds.Width;
	auto viewportHeight = graphicsContext->Viewport().Bounds.Height;

	auto translation = Matrix4x4::CreateTranslation(Vector3(-viewportWidth/2, viewportHeight/2, 0));
	
	spriteBatch->Begin(translation);
	{
		SpriteBatchDrawingContext drawingContext(*spriteBatch, pomdogTexture);
		hierarchy.Draw(drawingContext);
	}
	spriteBatch->End();
}
//-----------------------------------------------------------------------
void LightningTestGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(editorColorScheme.Background);
	backgroundPlane->Draw();

	//graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	
	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		fxaa->Draw(*graphicsContext, renderTarget);
	}
	
	DrawGUI();
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
