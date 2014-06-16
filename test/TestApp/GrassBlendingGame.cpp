//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GrassBlendingGame.hpp"
#include <utility>
#include <Pomdog/Utility/MakeUnique.hpp>
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "SandboxHelper.hpp"

#include "TextureAtlasLoader.hpp"
#include "Skeletal2D/SkeletonDescLoader.hpp"
#include "Skeletal2D/AnimationTrack.hpp"
#include "Skeletal2D/AnimationLoader.hpp"
#include "Skeletal2D/SkeletonLoader.hpp"
#include "Skeletal2D/SkinLoader.hpp"
#include "Skeletal2D/SpriteAnimationLoader.hpp"
#include "Skeletal2D/SkeletonHelper.hpp"

namespace TestApp {
namespace {
//-----------------------------------------------------------------------
static SkeletonPose CreateSkeletonPoseBySkeleton(Skeleton const& skeleton)
{
	SkeletonPose skeletonPose;
	skeletonPose.LocalPose.reserve(skeleton.JointCount());
	for (auto & joint: skeleton) {
		skeletonPose.LocalPose.push_back(joint.BindPose);
	}

	skeletonPose.GlobalPose.resize(skeleton.JointCount());
	SkeletonHelper::ComputeGlobalPoseFromLocalPose(skeleton, skeletonPose);
	
	return std::move(skeletonPose);
}
//-----------------------------------------------------------------------
static void LogSkeletalInfo(Details::TexturePacker::TextureAtlas const& textureAtlas,
	Details::Skeletal2D::SkeletonDesc const& skeletonDesc)
{
	Log::Info(StringFormat("TextureAtlas.Pages = %ld", textureAtlas.pages.size()));
	Log::Info(StringFormat("TextureAtlas.Regions = %ld", textureAtlas.regions.size()));
	
	for (auto page: textureAtlas.pages)
	{
		Log::Info(StringFormat("Page.Name = %s", page.Name.c_str()));
	}
	
	for (auto region: textureAtlas.regions)
	{
		Log::Info(StringFormat("Region.Name = %s", region.Name.c_str()));
		Log::Info(StringFormat("  Region.Page = %d", region.TexturePage));
		Log::Info(StringFormat("  Region.Rotate = %d", region.Rotate?1:0));
		Log::Info(StringFormat("  Region.Flip = %d", region.Flip?1:0));
		Log::Info(StringFormat("  Region.XY = %d, %d", (int)region.Subrect.X, (int)region.Subrect.Y));
		Log::Info(StringFormat("  Region.Size = %d, %d", (int)region.Subrect.Width, (int)region.Subrect.Height));
		Log::Info(StringFormat("  Region.Offset = %d %d", (int)region.XOffset, (int)region.YOffset));
		Log::Info(StringFormat("  Region.OriginalSize = %d %d", (int)region.OriginalWidth, (int)region.OriginalHeight));
	}

	Log::Info("-------------------------");
	for (auto & bone: skeletonDesc.Bones)
	{
		Log::Info(StringFormat("MaidChan/%s", bone.Name.c_str()));
		Log::Info(StringFormat("  Bone.Translate = %f, %f", bone.Pose.Translate.X, bone.Pose.Translate.Y));
		Log::Info(StringFormat("  Bone.Rotation = %f", bone.Pose.Rotation.value));
		Log::Info(StringFormat("  Bone.Scale = %f", bone.Pose.Scale));
	}
	
	Log::Info("-------------------------");
	for (auto & animationClip: skeletonDesc.AnimationClips)
	{
		for (auto & track: animationClip.BoneTracks)
		{
			Log::Info(StringFormat("AnimationClip/%s/Bones/%s", animationClip.Name.c_str(), track.BoneName.c_str()));
			for (auto & sample: track.TranslateSamples)
			{
				Log::Info(StringFormat("  {time: %f, x: %f, y: %f},",
					sample.TimeSeconds,
					sample.TranslateX.ToFloat(),
					sample.TranslateY.ToFloat()));
			}
			for (auto & sample: track.RotateSamples)
			{
				Log::Info(StringFormat("  {time: %f, rotate: %f},",
					sample.TimeSeconds,
					sample.Rotation.ToFloat()));
			}
			for (auto & sample: track.ScaleSamples)
			{
				Log::Info(StringFormat("  {time: %f, rotate: %f},",
					sample.TimeSeconds,
					sample.Scale.ToFloat()));
			}
		}
		for (auto & track: animationClip.SlotTracks)
		{
			Log::Info(StringFormat("AnimationClip/%s/Slots/%s", animationClip.Name.c_str(), track.SlotName.c_str()));
			for (auto & sample: track.AttachmentSamples)
			{
				Log::Info(StringFormat("  {time: %f, name: %s},",
					sample.TimeSeconds,
					sample.AttachmentName.c_str()));
			}
		}
	}
}

}// unnamed namespace
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
	window->AllowPlayerResizing(false);
	
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
	
	primitiveAxes = MakeUnique<SceneEditor::PrimitiveAxes>(gameHost, editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ);
	primitiveGrid = MakeUnique<SceneEditor::PrimitiveGrid>(gameHost, editorColorScheme.GuideLine, editorColorScheme.Grid);
	spriteRenderer = MakeUnique<SpriteRenderer>(graphicsContext, graphicsDevice, *assets);
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
		auto textureAtlas = assets->Load<Details::TexturePacker::TextureAtlas>("MaidChan2/skeleton.atlas");
		auto skeletonDesc = assets->Load<Details::Skeletal2D::SkeletonDesc>("MaidChan2/skeleton.json");
		maidTexture = assets->Load<Texture2D>("MaidChan2/skeleton.png");
		
		LogSkeletalInfo(textureAtlas, skeletonDesc);
		
		maidSkeleton = std::make_shared<Skeleton>(Details::Skeletal2D::CreateSkeleton(skeletonDesc.Bones));
		maidSkeletonPose = std::make_shared<SkeletonPose>(CreateSkeletonPoseBySkeleton(*maidSkeleton));
		auto animationClip = std::make_shared<AnimationClip>(Details::Skeletal2D::CreateAnimationClip(skeletonDesc, "Walk"));
		maidAnimationState = std::make_shared<AnimationState>(animationClip, 1.0f, true);
		
		maidSkin = Details::Skeletal2D::CreateSkin(skeletonDesc, textureAtlas, "default");
		maidSpriteAnimationTracks = Details::Skeletal2D::CreateSpriteAnimationTrack(skeletonDesc, textureAtlas, "Walk");
		
		animationSystem.Add(maidAnimationState, maidSkeleton, maidSkeletonPose);
	}
	
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
}
//-----------------------------------------------------------------------
void GrassBlendingGame::Update()
{
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		hierarchy.Touch(mouse->State());
	}
	{
		static auto duration = DurationSeconds(0);
		
		if (clock->TotalGameTime() - duration > DurationSeconds(0.2)) {
			gameHost->Window()->Title(StringFormat("%f fps", clock->FrameRate()));
			duration = clock->TotalGameTime();
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
	
	animationSystem.Update(*clock);
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
	auto transform = mainCamera->Component<Transform2D>();
	auto camera = mainCamera->Component<Camera2D>();
		
	POMDOG_ASSERT(transform && camera);
	auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);;
	auto projectionMatrix = Matrix4x4::CreateOrthographicLH(800.0f, 480.0f, 0.1f, 1000.0f);
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, viewMatrix * projectionMatrix);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, viewMatrix * projectionMatrix);

	POMDOG_ASSERT(spriteRenderer);
	spriteRenderer->Begin(SpriteSortMode::BackToFront, viewMatrix);

	auto const& globalPoses = maidSkeletonPose->GlobalPose;

	static int state = 3;
	static auto time = gameHost->Clock()->TotalGameTime();
	
	if (gameHost->Mouse()->State().RightButton == ButtonState::Pressed
		&& (gameHost->Clock()->TotalGameTime() - time > DurationSeconds(0.5))) {
		state += 1;
		state = state % 4;
		time = gameHost->Clock()->TotalGameTime();
	}
	
	if (state != 1)
	{
		for (auto & joint: *maidSkeleton)
		{
			auto & matrix = globalPoses[*joint.Index];
			spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 5, 5},
				Color::Black, MathConstants<float>::PiOver4(), {0.5f, 0.5f}, 1.0f, 2/100.0f);
			spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 2, 2},
				Color::White, MathConstants<float>::PiOver4(), {0.5f, 0.5f}, 1.0f, 1/100.0f);
			spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 32, 2},
				Color::Black, 0.0f, {0.0f, 0.5f}, 1.0f, 4/100.0f);
		}
	}

	if (state == 1 || state == 3)
	{
		for (auto & slot: maidSkin.Slots())
		{
			spriteRenderer->Draw(maidTexture, globalPoses[*slot.JointIndex], slot.Translate, slot.Subrect,
				slot.Color, (slot.TextureRotate ? slot.Rotation - MathConstants<float>::PiOver2(): slot.Rotation),
				slot.Origin, slot.Scale, (maidSkin.Slots().size() - slot.DrawOrder) / maidSkin.Slots().size());
		}
	}
	
	spriteRenderer->End();
	
	if (state == 2 || state == 3)
	{
		RasterizerDescription rasterizerDesc;
		rasterizerDesc.FillMode = FillMode::WireFrame;
		auto rasterizerState = std::make_shared<RasterizerState>(gameHost->GraphicsDevice(), rasterizerDesc);
		
		graphicsContext->SetRasterizerState(rasterizerState);
		spriteRenderer->Begin(SpriteSortMode::BackToFront, viewMatrix);
		
		for (auto & slot: maidSkin.Slots())
		{
			spriteRenderer->Draw(texture, globalPoses[*slot.JointIndex], slot.Translate, slot.Subrect,
				{0, 0, 0, 40},
				(slot.TextureRotate ? slot.Rotation - MathConstants<float>::PiOver2(): slot.Rotation),
				slot.Origin, slot.Scale, (maidSkin.Slots().size() - slot.DrawOrder) / maidSkin.Slots().size());
		}
		
		spriteRenderer->End();

		graphicsContext->SetRasterizerState(RasterizerState::CreateCullCounterClockwise(gameHost->GraphicsDevice()));
	}
}
//-----------------------------------------------------------------------
void GrassBlendingGame::DrawGUI()
{
//	POMDOG_ASSERT(spriteBatch);
//	auto viewportWidth = graphicsContext->Viewport().Bounds.Width;
//	auto viewportHeight = graphicsContext->Viewport().Bounds.Height;
//
//	auto translation = Matrix3x3::CreateTranslation(Vector2(-viewportWidth/2, viewportHeight/2));
//	
//	spriteBatch->Begin(translation);
//	{
//		SpriteBatchDrawingContext drawingContext(*spriteBatch, pomdogTexture);
//		hierarchy.Draw(drawingContext);
//	}
//	spriteBatch->End();
}
//-----------------------------------------------------------------------
void GrassBlendingGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(editorColorScheme.Background);
	backgroundPlane->Draw();
	
	graphicsContext->SetSamplerState(0, samplerPoint);
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
