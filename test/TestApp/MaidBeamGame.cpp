//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "MaidBeamGame.hpp"
#include <utility>
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"
#include "UI/StackPanel.hpp"
#include "UI/DebugNavigator.hpp"
#include "Graphics/SpriteLine.hpp"
#include "Rendering/Renderer.hpp"
#include "2D/Animator.hpp"
#include "2D/BeamRenderable.hpp"
#include "2D/SkinnedMeshRenderable.hpp"
#include "Spine/SkeletonDescLoader.hpp"
#include "Spine/SkeletonLoader.hpp"
#include "Spine/AnimationLoader.hpp"
#include "Spine/SkinnedMeshLoader.hpp"
#include "TexturePacker/TextureAtlasLoader.hpp"
#include "Skeletal2D/SkeletonHelper.hpp"
#include "Skeletal2D/AnimationClip.hpp"
#include "Skeletal2D/AnimationClipNode.hpp"
#include "Skeletal2D/AnimationLerpNode.hpp"
#include "Utilities/LogSkeletalInfo.hpp"


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
	window->Title("MaidBeamGame - Enjoy Game Dev, Have Fun.");
	window->AllowPlayerResizing(true);
	
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();

	{
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
		spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice, *assets);
		fxaa = std::make_unique<FXAA>(gameHost);
		polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
	}
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}

	{
		mainCamera = gameWorld.CreateObject();
		mainCamera->AddComponent<Transform2D>();
		mainCamera->AddComponent<Camera2D>();
	}
	{
		maid = gameWorld.CreateObject();
		maid->AddComponent<Transform2D>();
		
		auto skeletonDesc = assets->Load<Details::Skeletal2D::SkeletonDesc>("MaidGun/MaidGun.json");
		TestApp::LogSkeletalInfo(skeletonDesc);
		
		auto skeleton = std::make_shared<Skeleton>(Details::Skeletal2D::CreateSkeleton(skeletonDesc.Bones));

		auto skeletonTransform = std::make_shared<SkeletonTransform>();
		skeletonTransform->Pose = SkeletonPose::CreateBindPose(*skeleton);
		skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose);

		{
			auto animationClipRun = std::make_shared<AnimationClip>(Details::Skeletal2D::CreateAnimationClip(skeletonDesc, "Run"));
			auto animationClipRunNoShot = std::make_shared<AnimationClip>(Details::Skeletal2D::CreateAnimationClip(skeletonDesc, "RunNoShot"));

			auto clipNode1 = std::make_unique<AnimationClipNode>(animationClipRun);
			auto clipNode2 = std::make_unique<AnimationClipNode>(animationClipRunNoShot);
			
			std::uint16_t weightIndex = 0;
			auto lerpNode = std::make_unique<AnimationLerpNode>(std::move(clipNode1), std::move(clipNode2), weightIndex);
			
			auto animationGraph = std::make_shared<AnimationGraph>();
			animationGraph->Tree = std::move(lerpNode);
			animationGraph->Inputs = {
				{"lerpNode.Weight", AnimationBlendInputType::Float} // NOTE: weightIndex = 0
			};
			
			maid->AddComponent(std::make_unique<MaidAnimator>(skeleton, skeletonTransform, animationGraph));
		}
		{
			auto textureAtlas = assets->Load<Details::TexturePacker::TextureAtlas>("MaidGun/MaidGun.atlas");
			auto maidTexture = assets->Load<Texture2D>("MaidGun/MaidGun.png");
		
			TestApp::LogTexturePackerInfo(textureAtlas);

			auto bindPose = SkeletonPose::CreateBindPose(*skeleton);
			auto mesh = std::make_shared<SkinnedMesh>(Details::Skeletal2D::CreateSkinnedMesh(graphicsDevice,
				SkeletonHelper::ToGlobalPose(*skeleton, bindPose),
				skeletonDesc, textureAtlas,
				Vector2(maidTexture->Width(), maidTexture->Height()), "default"));

			maid->AddComponent(std::make_unique<SkinnedMeshRenderable>(graphicsDevice, assets, skeleton, skeletonTransform, mesh, maidTexture));
		}
	}
	{
		lightningBeam = gameWorld.CreateObject();
		lightningBeam->AddComponent<Transform2D>();
		auto & renderer = lightningBeam->AddComponent(std::make_unique<BeamRenderable>());
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
		spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(bounds.Width, bounds.Height, 1.0f, 100.0f));
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
		animator->SetFloat("lerpNode.Weight", slider2->Value());
		
//		if (toggleSwitch1->IsOn()) {
//			maidAnimationState->Stop();
//		}
	}

	if (auto renderable = maid->Component<Renderable>())
	{
		renderable->SetVisible(toggleSwitch2->IsOn());
		
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
	Renderer renderer;
	{
		auto transform = mainCamera->Component<Transform2D>();
		auto camera = mainCamera->Component<Camera2D>();
			
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			gameHost->Window()->ClientBounds().Width, gameHost->Window()->ClientBounds().Height, camera->Near, camera->Far);
		
		gameEditor->SetViewProjection(viewMatrix * projectionMatrix);

		for (auto & gameObject: gameWorld.QueryComponents<Renderable, Transform2D>())
		{
			auto renderable = gameObject->Component<Renderable>();
			renderable->Visit(*gameObject, renderer.renderQueue, viewMatrix, projectionMatrix);
		}
	}
	
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	gameEditor->BeginDraw(*graphicsContext);
	renderer.Render(graphicsContext);

	if (enableFxaa) {
		graphicsContext->SetRenderTarget();
		fxaa->Draw(*graphicsContext, renderTarget);
	}

	gameEditor->EndDraw(*graphicsContext);
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
