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
#include "SpriteBatch.hpp"
#include "SpriteRenderer.hpp"
#include "FXAA.hpp"
#include "TextureAtlasLoader.hpp"
#include "Skeletal2D/SkeletonDescLoader.hpp"
#include "Skeletal2D/AnimationTimeline.hpp"
#include "Skeletal2D/AnimationLoader.hpp"
#include "Skeletal2D/SkeletonLoader.hpp"
#include "Skeletal2D/SkinLoader.hpp"

namespace TestApp {
namespace {

//static Matrix3x3 CreateViewMatrix2D(Transform2D const& transform, Camera2D const& camera)
//{
//	return Matrix3x3::CreateTranslation({-transform.Position.X, -transform.Position.Y})*
//		Matrix3x3::CreateRotationZ(-transform.Rotation) *
//		Matrix3x3::CreateScale({camera.Zoom(), camera.Zoom(), 1});
//}
//-----------------------------------------------------------------------
static Matrix4x4 CreateViewMatrix3D(Transform2D const& transform, Camera2D const& camera)
{
	return Matrix4x4::CreateTranslation({-transform.Position.X, -transform.Position.Y, 1.0f})*
		Matrix4x4::CreateRotationZ(-transform.Rotation) *
		Matrix4x4::CreateScale({camera.Zoom(), camera.Zoom(), 1});
}
//-----------------------------------------------------------------------
static void Traverse(Matrix4x4 const& worldMatrix, std::vector<Joint> const& bones,
	JointIndex const& boneIndex,
	std::function<void(Matrix4x4 const& worldMatrix, Matrix4x4 const& boneMatrix, Joint const&)> const& traverser)
{
	POMDOG_ASSERT(boneIndex);
	POMDOG_ASSERT(*boneIndex < bones.size());
	auto & bone = bones[*boneIndex];

	Matrix4x4 matrix = Matrix4x4::CreateScale(bone.Scale);
	matrix *= Matrix4x4::CreateRotationZ(bone.Rotation);
	matrix *= Matrix4x4::CreateTranslation({bone.Translate, 0.0f});
	matrix *= worldMatrix;

	if (bone.FirstChild) {
		Traverse(matrix, bones, bone.FirstChild, traverser);
	}
	if (bone.Sibling) {
		Traverse(worldMatrix, bones, bone.Sibling, traverser);
	}
	traverser(worldMatrix, matrix, bone);
}
//-----------------------------------------------------------------------
static void Traverse(std::vector<Joint> const& bones,
	std::function<void(Matrix4x4 const& worldMatrix, Matrix4x4 const& boneMatrix, Joint const&)> const& traverser)
{
	Traverse(Matrix4x4::Identity, bones, bones.front().Index, traverser);
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
		for (auto & sample: animationClip.Samples)
		{
			Log::Info(StringFormat("AnimationClip/%s/%s", animationClip.Name.c_str(), sample.BoneName.c_str()));
			for (auto & translateSample: sample.TranslateSamples)
			{
				Log::Info(StringFormat("  {time: %f, x: %f, y: %f},",
					translateSample.TimeSeconds,
					translateSample.TranslateX.ToFloat(),
					translateSample.TranslateY.ToFloat()));
			}
			for (auto & rotateSample: sample.RotateSamples)
			{
				Log::Info(StringFormat("  {time: %f, rotate: %f},",
					rotateSample.TimeSeconds,
					rotateSample.Rotation.ToFloat()));
			}
		}
	}
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
	
	primitiveAxes = std::unique_ptr<PrimitiveAxes>(new PrimitiveAxes(gameHost));
	primitiveGrid = std::unique_ptr<PrimitiveGrid>(new PrimitiveGrid(gameHost));
	spriteRenderer = std::unique_ptr<SpriteRenderer>(new SpriteRenderer(graphicsContext, graphicsDevice, *assets));
	fxaa = std::unique_ptr<FXAA>(new FXAA(gameHost));
	
	{
		// NOTE: Create main camera:
		auto gameObject = gameWorld.CreateObject();;
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
		auto textureAtlas = assets->Load<Details::TexturePacker::TextureAtlas>("MaidChan/parts.atlas");
		auto skeletonDesc = assets->Load<Details::Skeletal2D::SkeletonDesc>("MaidChan/skeleton.json");
		
		maidSkeleton = Details::Skeletal2D::CreateSkeleton(skeletonDesc.Bones);
		auto skinName = "default";
		maidSkin = Details::Skeletal2D::CreateSkin(skeletonDesc, textureAtlas, skinName);
		maidAnimation = Details::Skeletal2D::CreateAnimationClip(skeletonDesc);
		maidTexture = assets->Load<Texture2D>("MaidChan/parts.png");

		Traverse(maidSkeleton.Joints(), [&](Matrix4x4 const& matrix, Matrix4x4 const& boneMatrix, Joint const& bone) {
			POMDOG_ASSERT(*bone.Index < maidSkeleton.GlobalPoses().size());
			maidSkeleton.GlobalPoses()[*bone.Index] = boneMatrix;
		});

		LogSkeletalInfo(textureAtlas, skeletonDesc);
	}
}
//-----------------------------------------------------------------------
void TestAppGame::Update()
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
	
	if (auto rootNode = gameWorld.Component<Node2D>(rootObjectID))
	{
		auto & transform = rootNode->Transform();
		if (mouse->State().RightButton == ButtonState::Pressed) {
			transform.Position.Y += 0.5f;
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
		static auto totalTime = DurationSeconds(0);
		totalTime += clock->FrameDuration();
		
		maidAnimation.Apply(maidSkeleton, totalTime);
		
		if (totalTime > DurationSeconds(1.3)) {
			totalTime = DurationSeconds(0);
		}

		Traverse(maidSkeleton.Joints(), [&](Matrix4x4 const& matrix, Matrix4x4 const& boneMatrix, Joint const& bone) {
			POMDOG_ASSERT(*bone.Index < maidSkeleton.GlobalPoses().size());
			maidSkeleton.GlobalPoses()[*bone.Index] = boneMatrix;
		});
	}
}
//-----------------------------------------------------------------------
void TestAppGame::DrawSprites()
{
	auto camera = gameWorld.Component<Camera2D>(mainCameraID);
	auto nodeCamera = gameWorld.Component<Node2D>(mainCameraID);
	
	auto vierMatrix3D = CreateViewMatrix3D(nodeCamera->Transform(), *camera);;
	auto projectionMatrix3D = Matrix4x4::CreateOrthographicLH(800.0f, 480.0f, 0.1f, 1000.0f);
	
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(*graphicsContext, vierMatrix3D * projectionMatrix3D);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(*graphicsContext, vierMatrix3D * projectionMatrix3D);
	
	//auto viewMatrix2D = CreateViewMatrix2D(nodeCamera->Transform(), *camera);
	
	POMDOG_ASSERT(spriteRenderer);
	spriteRenderer->Begin(vierMatrix3D);
	
//	auto rootNode = gameWorld.Component<Node2D>(rootObjectID);
//	auto worldMatrix = Matrix4x4::CreateTranslation(Vector3(rootNode->Transform().Position, 0.0f));
//	{
//		for (auto child: rootNode->Children())
//		{
//			auto canvasItem = child->Component<CanvasItem>();
//			if (!child || !canvasItem->Visibile) {
//				continue;
//			}
//		
//			auto node = child->Component<Node2D>();
//
//			auto sprite = child->Component<Sprite>();
//			auto & transform = node->Transform();
//			
//			constexpr float layerDepth = 0.0f;
//
//			spriteRenderer->Draw(texture, worldMatrix, transform.Position, sprite->Subrect,
//				Color::White, transform.Rotation, sprite->Origin, transform.Scale, layerDepth);
//		}
//	}

	auto const& globalPoses = maidSkeleton.GlobalPoses();

	for (auto & joint: maidSkeleton.Joints())
	{
		auto & matrix = globalPoses[*joint.Index];
		spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 5, 5},
			Color::Black, MathConstants<float>::PiOver4(), {0.5f, 0.5f}, 1.0f, 2/100.0f);
		spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 2, 2},
			Color::White, MathConstants<float>::PiOver4(), {0.5f, 0.5f}, 1.0f, 1/100.0f);
		spriteRenderer->Draw(texture, matrix, Vector2::Zero, {0, 0, 32, 2},
			Color::Black, 0.0f, {0.0f, 0.5f}, 1.0f, 4/100.0f);
	}
	
	static int state = 3;
	static auto time = gameHost->Clock()->TotalGameTime();
	
	if (gameHost->Mouse()->State().RightButton == ButtonState::Pressed
		&& (gameHost->Clock()->TotalGameTime() - time > DurationSeconds(0.5))) {
		state += 1;
		state = state % 4;
		time = gameHost->Clock()->TotalGameTime();
	}

	if (state == 1 || state == 3) {
		
		for (auto & slot: maidSkin.Slots())
		{
			spriteRenderer->Draw(maidTexture, globalPoses[slot.BoneIndex], slot.Translate, slot.Subrect,
				slot.Color, (slot.TextureRotate ? slot.Rotation - MathConstants<float>::PiOver2(): slot.Rotation),
				slot.Origin, slot.Scale, (maidSkin.Slots().size() - slot.DrawOrder) / maidSkin.Slots().size());
		}
	}
	
	spriteRenderer->End();
	
	if (state == 2 || state == 3) {
		
		//
		RasterizerDescription rasterizerDesc;
		rasterizerDesc.FillMode = FillMode::WireFrame;
		auto rasterizerState = std::make_shared<RasterizerState>(gameHost->GraphicsDevice(), rasterizerDesc);
		
		graphicsContext->SetRasterizerState(rasterizerState);
		spriteRenderer->Begin(vierMatrix3D);
		
		for (auto & slot: maidSkin.Slots())
		{
			spriteRenderer->Draw(texture, globalPoses[slot.BoneIndex], slot.Translate, slot.Subrect,
				{0,0,0,40},
				(slot.TextureRotate ? slot.Rotation - MathConstants<float>::PiOver2(): slot.Rotation),
				slot.Origin, slot.Scale, (maidSkin.Slots().size() - slot.DrawOrder) / maidSkin.Slots().size());
		}
		
		spriteRenderer->End();

		graphicsContext->SetRasterizerState(RasterizerState::CreateCullCounterClockwise(gameHost->GraphicsDevice()));
	}
}
//-----------------------------------------------------------------------
void TestAppGame::Draw()
{
	constexpr bool enableFxaa = true;

	if (enableFxaa) {
		graphicsContext->SetRenderTarget(renderTarget);
	}
	
	graphicsContext->Clear(Color::CornflowerBlue);
	
	graphicsContext->SetSamplerState(0, samplerPoint);
	DrawSprites();
	
	if (enableFxaa) {
		fxaa->Draw(*graphicsContext, renderTarget);
	}
	
	graphicsContext->Present();
}
//-----------------------------------------------------------------------
}// namespace TestApp
