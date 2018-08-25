// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GunShootingLevel.hpp"
#include "../Utilities/LogSkeletalInfo.hpp"

namespace TestApp {
namespace {

class Ghost: public Component<Ghost> {
public:

};

void LoadAnimator(GameObject & gameObject, std::shared_ptr<GraphicsDevice> const& graphicsDevice,
    AssetManager & assets)
{
    auto skeletonDesc = Spine::SkeletonDescLoader::Load(assets, "MaidGun/MaidGun.json");
    TestApp::LogSkeletalInfo(skeletonDesc);

    auto skeleton = std::make_shared<Skeleton>(Spine::CreateSkeleton(skeletonDesc.Bones));

    auto skeletonTransform = std::make_shared<SkeletonTransform>();
    skeletonTransform->Pose = SkeletonPose::CreateBindPose(*skeleton);
    skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*skeleton, skeletonTransform->Pose);
    {
        auto animationGraph = Spine::LoadAnimationGraph(skeletonDesc, assets, "MaidGun/AnimationGraph.json");
        gameObject.AddComponent(std::make_unique<Animator>(skeleton, skeletonTransform, animationGraph));
    }
    {
        auto textureAtlas = TexturePacker::TextureAtlasLoader::Load(assets, "MaidGun/MaidGun.atlas");
        auto texture = assets.Load<Texture2D>("MaidGun/MaidGun.png");

        TestApp::LogTexturePackerInfo(textureAtlas);

        auto bindPose = SkeletonPose::CreateBindPose(*skeleton);
        auto mesh = std::make_shared<SkinnedMesh>(Spine::CreateSkinnedMesh(*graphicsDevice,
            SkeletonHelper::ToGlobalPose(*skeleton, bindPose),
            skeletonDesc, textureAtlas,
            Vector2(texture->Width(), texture->Height()), "default"));

        gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(
            skeleton, skeletonTransform, mesh, texture));
    }
}

}// unnamed namespace

GunShootingLevel::GunShootingLevel(GameHost & gameHost, GameWorld & world)
    : spawnTime(Duration::zero())
{
    auto graphicsDevice = gameHost.GraphicsDevice();
    auto assets = gameHost.AssetManager();
//    {
//        for (auto & cameraObject: world.QueryComponents<Camera2D>())
//        {
//            auto camera = cameraObject.Component<Camera2D>();
//            camera->Enabled = false;
//        }
//    }
    {
        mainCamera = world.CreateObject();
        auto & transform = mainCamera.AddComponent<Transform2D>();
        transform.Position = {0.0f, 400.0f};
        auto & camera = mainCamera.AddComponent<Camera2D>();
        camera.Zoom = 0.60f;
        //mainCamera.AddComponent<ScriptBehavior>(*assets, "Scripts/Maid.lua");
    }
    {
        maid = world.CreateObject();
        maid.AddComponent<Transform2D>();
        LoadAnimator(maid, graphicsDevice, *assets);
        auto animator = maid.Component<Animator>();
        animator->PlaybackRate(1.5f);
        //maid.AddComponent<ScriptBehavior>(*assets, "Scripts/Maid.lua");
    }
    {
        lightningBeam = world.CreateObject();
        lightningBeam.AddComponent<Transform2D>();
        auto & rendererable = lightningBeam.AddComponent(std::make_unique<BeamRenderable>());
        rendererable.Load(graphicsDevice, assets);
    }
    {
        auto skeletonDesc = Spine::SkeletonDescLoader::Load(*assets, "Ghost/Ghost.json");
        TestApp::LogSkeletalInfo(skeletonDesc);

        ghostSkeleton = std::make_shared<Skeleton>(Spine::CreateSkeleton(skeletonDesc.Bones));
        ghostAnimGraph = Spine::LoadAnimationGraph(skeletonDesc, *assets, "Ghost/AnimationGraph.json");
        {
            auto textureAtlas = TexturePacker::TextureAtlasLoader::Load(*assets, "Ghost/Ghost.atlas");
            TestApp::LogTexturePackerInfo(textureAtlas);

            ghostTexture = assets->Load<Texture2D>("Ghost/Ghost.png");

            auto bindPose = SkeletonPose::CreateBindPose(*ghostSkeleton);
            ghostMesh = std::make_shared<SkinnedMesh>(Spine::CreateSkinnedMesh(*graphicsDevice,
                SkeletonHelper::ToGlobalPose(*ghostSkeleton, bindPose),
                skeletonDesc, textureAtlas,
                Vector2(ghostTexture->Width(), ghostTexture->Height()), "default"));
        }
    }
}

void GunShootingLevel::Update(GameHost & gameHost, GameWorld & world)
{
    auto clock = gameHost.Clock();
    auto frameDuration = clock->FrameDuration();
    spawnTime += frameDuration;

    if (spawnTime > Duration{2.0})
    {
        auto graphicsDevice = gameHost.GraphicsDevice();
        auto assets = gameHost.AssetManager();

        for (int i = 0; i < 5; ++i)
        {
            auto gameObject = world.CreateObject();
            auto & transform = gameObject.AddComponent<Transform2D>();
            transform.Position = {600.0f, 150.0f * i + 70.0f};

            auto skeletonTransform = std::make_shared<SkeletonTransform>();
            skeletonTransform->Pose = SkeletonPose::CreateBindPose(*ghostSkeleton);
            skeletonTransform->GlobalPose = SkeletonHelper::ToGlobalPose(*ghostSkeleton, skeletonTransform->Pose);
            gameObject.AddComponent(std::make_unique<Animator>(ghostSkeleton, skeletonTransform, ghostAnimGraph));
            gameObject.AddComponent(std::make_unique<SkinnedMeshRenderable>(
                ghostSkeleton, skeletonTransform, ghostMesh, ghostTexture));

            gameObject.AddComponent<Ghost>();
        }
        spawnTime = Duration::zero();
    }

    for (auto & ghost: world.QueryComponents<Transform2D, Ghost>())
    {
        auto transform = ghost.Component<Transform2D>();
        transform->Position.X -= 200.0f * frameDuration.count();

        if (transform->Position.X < -300.0f)
        {
            ghost.Destroy();
        }
    }

//    {
//        auto keyboard = gameHost->Keyboard();
//
//        if (auto transform = maid->Component<Transform2D>())
//        {
//            Vector2 velocity = Vector2::Zero;
//
//            if (keyboard->State().IsKeyDown(Keys::A)) {
//                velocity.X -= 300.0f;
//            }
//            if (keyboard->State().IsKeyDown(Keys::D)) {
//                velocity.X += 300.0f;
//            }
//            if (keyboard->State().IsKeyDown(Keys::W)) {
//                velocity.Y += 50.0f;
//            }
//            if (keyboard->State().IsKeyDown(Keys::S)) {
//                velocity.Y -= 50.0f;
//            }
//
//            transform->Position = transform->Position + (velocity * clock->FrameDuration().count());
//        }
//    }

//    if (auto animator = maid.Component<Animator>())
//    {
//        animator->PlaybackRate(slider1->Value());
//        animator->SetFloat("Run.Weight", slider2->Value());
//
//        if (!toggleSwitch1->IsOn()) {
//            animator->PlaybackRate(0.0f);
//        }
//    }
//
//    if (auto renderable = maid.Component<Renderable>())
//    {
//        renderable->IsVisible = toggleSwitch2->IsOn();
//
////        if (toggleSwitch3->IsOn()) {
////            renderer->SkeletonDebugDrawEnable = true;
////        }
////        else {
////            renderer->SkeletonDebugDrawEnable = false;
////        }
////
////        if (toggleSwitch4->IsOn()) {
////            renderer->MeshWireframeEnable = true;
////        }
////        else {
////            renderer->MeshWireframeEnable = false;
////        }
//    }
}

}// namespace TestApp
