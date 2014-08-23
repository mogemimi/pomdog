//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GrassBlendingGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include "Utilities/LogSkeletalInfo.hpp"
#include <utility>

namespace TestApp {
//-----------------------------------------------------------------------
GrassBlendingGame::GrassBlendingGame(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	POMDOG_ASSERT(gameHostIn);
	graphicsContext = gameHost->GraphicsContext();
}
//-----------------------------------------------------------------------
GrassBlendingGame::~GrassBlendingGame() = default;
//-----------------------------------------------------------------------
void GrassBlendingGame::Initialize()
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
		spriteRenderer = std::make_unique<SpriteRenderer>(graphicsContext, graphicsDevice);
		fxaa = std::make_unique<FXAA>(graphicsDevice);
		auto bounds = window->ClientBounds();
		fxaa->SetViewport(bounds.Width, bounds.Height);
		polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice);
	}
	{
		gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
	}
	
	{
		mainCamera = gameWorld.CreateObject();
		mainCamera.AddComponent<Transform2D>();
		mainCamera.AddComponent<Camera2D>();
	}
	{
		auto textureAtlas = assets->Load<Details::TexturePacker::TextureAtlas>("MaidChan2/skeleton.atlas");
		auto skeletonDesc = assets->Load<Details::Spine::SkeletonDesc>("MaidChan2/skeleton.json");
		maidTexture = assets->Load<Texture2D>("MaidChan2/skeleton.png");
		
		LogTexturePackerInfo(textureAtlas);
		LogSkeletalInfo(skeletonDesc);
		
		maidSkeleton = std::make_shared<Skeleton>(Details::Spine::CreateSkeleton(skeletonDesc.Bones));
		maidSkeletonPose = std::make_shared<SkeletonPose>(SkeletonPose::CreateBindPose(*maidSkeleton));
		auto animationClip = std::make_shared<AnimationClip>(Details::Spine::CreateAnimationClip(skeletonDesc, "Walk"));
		maidAnimationState = std::make_shared<AnimationState>(animationClip, 1.0f, true);
		maidAnimationClipIdle = std::make_shared<AnimationClip>(Details::Spine::CreateAnimationClip(skeletonDesc, "Idle"));
		
		maidSkin = Details::Spine::CreateSkin(skeletonDesc, textureAtlas, "default");
		maidSpriteAnimationTracks = Details::Spine::CreateSpriteAnimationTrack(skeletonDesc, textureAtlas, "Walk");
		
		animationSystem.Add(maidAnimationState, maidSkeleton, maidSkeletonPose);

		maidGlobalPose = SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose);

		// NOTE: for Skinning
		auto bindPose = SkeletonPose::CreateBindPose(*maidSkeleton);
		maidSkinnedMesh = Details::Spine::CreateSkinnedMesh(graphicsDevice,
			SkeletonHelper::ToGlobalPose(*maidSkeleton, bindPose),
			skeletonDesc, textureAtlas,
			Vector2(maidTexture->Width(), maidTexture->Height()), "default");
		maidSkinningEffect = std::make_unique<SkinnedEffect>(graphicsDevice);
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
			slider1 = std::make_shared<UI::Slider>(-2.0, 2.0);
			slider1->Value(1.0);
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
	
	clientSizeChangedConnection = window->ClientSizeChanged.Connect([this](int width, int height) {
		graphicsContext->Viewport(Viewport{0, 0, width, height});
		graphicsContext->ScissorRectangle(Rectangle{0, 0, width, height});
		
		renderTarget = std::make_shared<RenderTarget2D>(
			gameHost->GraphicsDevice(), width, height,
			false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

		fxaa->SetViewport(width, height);
		spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(width, height, 1.0f, 100.0f));
	});
}
//-----------------------------------------------------------------------
void GrassBlendingGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		gameEditor->Update();
	}
	{
		maidAnimationState->PlaybackRate(slider1->Value());
	}
	
	if (toggleSwitch1->IsOn())
	{
		animationSystem.Update(*clock);
		
		SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose, maidGlobalPose);
	
		{
//			///@note Test code for animation blending
//			auto clipNode1 = std::make_unique<AnimationClipNode>(maidAnimationState->Clip());
//			auto clipNode2 = std::make_unique<AnimationClipNode>(maidAnimationClipIdle);
//		
//			auto lerpNode = std::make_unique<AnimationLerpNode>(std::move(clipNode1), std::move(clipNode2));
//			lerpNode->Weight(slider2->Value());
//			
//			lerpNode->Calculate(maidAnimationState->Time(), *maidSkeleton, *maidSkeletonPose);
			
			SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose, maidGlobalPose);
		}
	}
	{
		maidAnimationTimer.Update(clock->FrameDuration());
		if (maidAnimationTimer.Time() > maidAnimationState->Length()) {
			maidAnimationTimer.Reset();
		}
	}
	{
		for (auto & track: maidSpriteAnimationTracks)
		{
			track.Apply(maidSkin, maidAnimationTimer.Time());
		}
	}
}
//-----------------------------------------------------------------------
void GrassBlendingGame::DrawSprites()
{
	auto transform = mainCamera.Component<Transform2D>();
	auto camera = mainCamera.Component<Camera2D>();
		
	POMDOG_ASSERT(transform && camera);
	auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
	auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
		gameHost->Window()->ClientBounds().Width, gameHost->Window()->ClientBounds().Height, 0.1f, 1000.0f);

	POMDOG_ASSERT(polygonBatch);
	polygonBatch->Begin(viewMatrix * projectionMatrix);

	auto const& globalPoses = maidGlobalPose;
	
	if (toggleSwitch3->IsOn())
	{
		Color boneColor {160, 160, 160, 255};
	
		for (auto & joint: *maidSkeleton)
		{
			auto & matrix = globalPoses[*joint.Index];

			if (maidSkeleton->Root().Index != joint.Index)
			{
				polygonBatch->DrawTriangle(
					Vector2::Transform({1.7f, -4.7f}, matrix),
					Vector2::Transform({1.7f, 4.7f}, matrix),
					Vector2::Transform({25, 0}, matrix), boneColor);
			}
			
			auto center = Vector2::Transform(Vector2::Zero, matrix);
			polygonBatch->DrawCircle(center, 5.0f, boneColor, 18);
			polygonBatch->DrawCircle(center, 3.0f, Color::White, 13);
		}
	}
	
	polygonBatch->End();
}
//-----------------------------------------------------------------------
void GrassBlendingGame::DrawSkinnedMesh()
{
	{
		auto transform = mainCamera.Component<Transform2D>();
		auto camera = mainCamera.Component<Camera2D>();
		
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			graphicsContext->Viewport().Width(), graphicsContext->Viewport().Height(), 0.1f, 1000.0f);
		
		maidSkinningEffect->SetWorldViewProjection(viewMatrix * projectionMatrix);

		std::array<Matrix3x2, 64> matrices;

		for (auto & joint: *maidSkeleton)
		{
			POMDOG_ASSERT(joint.Index);
			POMDOG_ASSERT(*joint.Index < matrices.size());
			POMDOG_ASSERT(*joint.Index < maidGlobalPose.size());
			matrices[*joint.Index] = joint.InverseBindPose * maidGlobalPose[*joint.Index];
		}

		maidSkinningEffect->SetBoneTransforms(matrices.data(), matrices.size());
		maidSkinningEffect->SetTexture(maidTexture);
	}
	
	if (toggleSwitch2->IsOn())
	{
		graphicsContext->SetVertexBuffer(maidSkinnedMesh.VertexBuffer);
		maidSkinningEffect->Apply(*graphicsContext);
		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
			maidSkinnedMesh.IndexBuffer, maidSkinnedMesh.IndexBuffer->IndexCount());
	}
	
	if (toggleSwitch4->IsOn())
	{
		RasterizerDescription rasterizerDesc;
		rasterizerDesc.FillMode = FillMode::WireFrame;
		auto rasterizerState = std::make_shared<RasterizerState>(gameHost->GraphicsDevice(), rasterizerDesc);
		
		graphicsContext->SetRasterizerState(rasterizerState);
		
		maidSkinningEffect->SetTexture(texture);
		graphicsContext->SetVertexBuffer(maidSkinnedMesh.VertexBuffer);
		maidSkinningEffect->Apply(*graphicsContext);
		graphicsContext->DrawIndexed(PrimitiveTopology::TriangleList,
									 maidSkinnedMesh.IndexBuffer, maidSkinnedMesh.IndexBuffer->IndexCount());
		
		graphicsContext->SetRasterizerState(RasterizerState::CreateCullCounterClockwise(gameHost->GraphicsDevice()));
	}
}
//-----------------------------------------------------------------------
void GrassBlendingGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	{
		auto transform = mainCamera.Component<Transform2D>();
		auto camera = mainCamera.Component<Camera2D>();
			
		POMDOG_ASSERT(transform && camera);
		auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
		auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
			graphicsContext->Viewport().Width(), graphicsContext->Viewport().Height(), 0.1f, 1000.0f);
		
		gameEditor->SetViewProjection(viewMatrix * projectionMatrix);
	}
	gameEditor->BeginDraw(*graphicsContext);

	graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	DrawSkinnedMesh();
	
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
