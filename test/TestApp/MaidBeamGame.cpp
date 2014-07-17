//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MaidBeamGame.hpp"
#include <utility>
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"
#include "Graphics/SpriteLine.hpp"
#include "2D/Animator.hpp"
#include "2D/BeamRenderable.hpp"
#include "2D/SkinnedMeshRenderable.hpp"
#include "2D/SpriteRenderable.hpp"
#include "Spine/SkeletonDescLoader.hpp"
#include "Spine/SkeletonLoader.hpp"
#include "Spine/SkinnedMeshLoader.hpp"
#include "TexturePacker/TextureAtlasLoader.hpp"
#include "Skeletal2D/SkeletonHelper.hpp"
#include "Skeletal2D/AnimationGraphBuilder.hpp"
#include "Utilities/LogSkeletalInfo.hpp"


namespace TestApp {

void LoadAnimator(GameObject & gameObject, std::shared_ptr<GraphicsDevice> const& graphicsDevice,
	AssetManager & assets)
{
	auto skeletonDesc = assets.Load<Details::Skeletal2D::SkeletonDesc>("MaidGun/MaidGun.json");
	TestApp::LogSkeletalInfo(skeletonDesc);
	
	auto skeleton = std::make_shared<Skeleton>(Details::Skeletal2D::CreateSkeleton(skeletonDesc.Bones));

	auto skeletonTransform = std::make_shared<SkeletonTransform>();
	skeletonTransform->Pose = SkeletonPose::CreateBindPose(*skeleton);
	skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose);

	{
		auto animationGraph = Details::Skeletal2D::LoadAnimationGraph(skeletonDesc, assets, "MaidGun/AnimationGraph.json");
		gameObject.AddComponent(std::make_unique<MaidAnimator>(skeleton, skeletonTransform, animationGraph));
	}
	{
		auto textureAtlas = assets.Load<Details::TexturePacker::TextureAtlas>("MaidGun/MaidGun.atlas");
		auto maidTexture = assets.Load<Texture2D>("MaidGun/MaidGun.png");
	
		TestApp::LogTexturePackerInfo(textureAtlas);

		auto bindPose = SkeletonPose::CreateBindPose(*skeleton);
		auto mesh = std::make_shared<SkinnedMesh>(Details::Skeletal2D::CreateSkinnedMesh(graphicsDevice,
			SkeletonHelper::ToGlobalPose(*skeleton, bindPose),
			skeletonDesc, textureAtlas,
			Vector2(maidTexture->Width(), maidTexture->Height()), "default"));

		gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(graphicsDevice, assets, skeleton, skeletonTransform, mesh, maidTexture));
	}
}

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
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}

	{
		mainCamera = gameWorld.CreateObject();
		mainCamera->AddComponent<Transform2D>();
		mainCamera->AddComponent<Camera2D>();
		
		auto texture = assets->Load<Texture2D>("pomdog.png");
		mainCamera->AddComponent<SpriteRenderable>(texture);
	}
	{
		maid = gameWorld.CreateObject();
		maid->AddComponent<Transform2D>();
		LoadAnimator(*maid, graphicsDevice, *assets);
	}
	{
		lightningBeam = gameWorld.CreateObject();
		lightningBeam->AddComponent<Transform2D>();
		auto & rendererable = lightningBeam->AddComponent(std::make_unique<BeamRenderable>());
		rendererable.Load(graphicsDevice, assets);
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
			textBlock1 = std::make_shared<UI::TextBlock>();
			textBlock1->Text("DrawCall: --");
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
	});
}
//-----------------------------------------------------------------------
void MaidBeamGame::Update()
{
	{
		gameEditor->Update();
	}
	
	auto clock = gameHost->Clock();

	for (auto & gameObject: gameWorld.QueryComponents<Animator>())
	{
		auto animator = gameObject->Component<Animator>();
		
		POMDOG_ASSERT(animator);
		animator->Update(*clock);
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

	if (auto animator = maid->Component<Animator>())
	{
		animator->PlaybackRate(slider1->Value());
		animator->SetFloat("Run.Weight", slider2->Value());
		
		if (!toggleSwitch1->IsOn()) {
			animator->PlaybackRate(0.0f);
		}
	}

	if (auto renderable = maid->Component<Renderable>())
	{
		renderable->IsVisible(toggleSwitch2->IsOn());
		
//		if (toggleSwitch3->IsOn()) {
//			renderer->SkeletonDebugDrawEnable = true;
//		}
//		else {
//			renderer->SkeletonDebugDrawEnable = false;
//		}
//		
//		if (toggleSwitch4->IsOn()) {
//			renderer->MeshWireframeEnable = true;
//		}
//		else {
//			renderer->MeshWireframeEnable = false;
//		}
	}

	gameWorld.RemoveUnusedObjects();
}
//-----------------------------------------------------------------------
void MaidBeamGame::Draw()
{
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
			
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
