//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GunShootingLevel.hpp"
#include "../2D/Animator.hpp"
#include "../2D/BeamRenderable.hpp"
#include "../2D/Behavior.hpp"
#include "../2D/ScriptBehavior.hpp"
#include "../2D/SkinnedMeshRenderable.hpp"
#include "../2D/SpriteRenderable.hpp"
#include "../Spine/SkeletonDescLoader.hpp"
#include "../Spine/SkeletonLoader.hpp"
#include "../Spine/SkinnedMeshLoader.hpp"
#include "../TexturePacker/TextureAtlasLoader.hpp"
#include "../Skeletal2D/SkeletonHelper.hpp"
#include "../Skeletal2D/AnimationGraphBuilder.hpp"
#include "../Utilities/LogSkeletalInfo.hpp"

namespace TestApp {
namespace {

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

}// unnamed namespace
//-----------------------------------------------------------------------
GunShootingLevel::GunShootingLevel(GameHost & gameHost, GameWorld & world)
{
	auto graphicsDevice = gameHost.GraphicsDevice();
	auto assets = gameHost.AssetManager();
	{
		for (auto & cameraObject: world.QueryComponents<Camera2D>())
		{
			auto camera = cameraObject.Component<Camera2D>();
			camera->Enabled = false;
		}
	}
	{
		mainCamera = world.CreateObject();
		mainCamera.AddComponent<Transform2D>();
		mainCamera.AddComponent<Camera2D>();
		mainCamera.AddComponent<ScriptBehavior>(*assets, "Scripts/Maid.lua");
	}
	{
		maid = world.CreateObject();
		maid.AddComponent<Transform2D>();
		LoadAnimator(maid, graphicsDevice, *assets);
		maid.AddComponent<ScriptBehavior>(*assets, "Scripts/Maid.lua");
	}
	{
		lightningBeam = world.CreateObject();
		lightningBeam.AddComponent<Transform2D>();
		auto & rendererable = lightningBeam.AddComponent(std::make_unique<BeamRenderable>());
		rendererable.Load(graphicsDevice, assets);
	}	
}
//-----------------------------------------------------------------------
void GunShootingLevel::Update(GameHost & gameHost)
{
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

//	if (auto animator = maid.Component<Animator>())
//	{
//		animator->PlaybackRate(slider1->Value());
//		animator->SetFloat("Run.Weight", slider2->Value());
//		
//		if (!toggleSwitch1->IsOn()) {
//			animator->PlaybackRate(0.0f);
//		}
//	}
//
//	if (auto renderable = maid.Component<Renderable>())
//	{
//		renderable->IsVisible = toggleSwitch2->IsOn();
//		
////		if (toggleSwitch3->IsOn()) {
////			renderer->SkeletonDebugDrawEnable = true;
////		}
////		else {
////			renderer->SkeletonDebugDrawEnable = false;
////		}
////		
////		if (toggleSwitch4->IsOn()) {
////			renderer->MeshWireframeEnable = true;
////		}
////		else {
////			renderer->MeshWireframeEnable = false;
////		}
//	}
}
//-----------------------------------------------------------------------
}// namespace TestApp
