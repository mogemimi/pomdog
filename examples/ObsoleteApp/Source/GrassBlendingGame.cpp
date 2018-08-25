// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GrassBlendingGame.hpp"
#include "Utilities/SandboxHelper.hpp"
#include "Utilities/LogSkeletalInfo.hpp"
#include <utility>

namespace TestApp {

GrassBlendingGame::GrassBlendingGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
{}

GrassBlendingGame::~GrassBlendingGame() = default;

void GrassBlendingGame::Initialize()
{
    window->SetTitle("TestApp - Enjoy Game Dev, Have Fun.");
    window->SetAllowUserResizing(true);

    auto assets = gameHost->AssetManager();
    auto clientBounds = window->GetClientBounds();

    {
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        samplerPoint = std::make_shared<SamplerState>(graphicsDevice,
            SamplerDescription::CreateLinearWrap());

        texture = std::make_shared<Texture2D>(graphicsDevice,
            1, 1, false, SurfaceFormat::R8G8B8A8_UNorm);

        std::array<std::uint32_t, 1> pixelData = {0xffffffff};
        texture->SetData(pixelData.data());

        renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
            clientBounds.Width, clientBounds.Height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);
    }
    {
        spriteRenderer = std::make_unique<SpriteRenderer>(graphicsDevice, *assets);
        fxaa = std::make_unique<FXAA>(graphicsDevice, *assets);
        fxaa->SetViewport(clientBounds.Width, clientBounds.Height);
        screenQuad = std::make_unique<ScreenQuad>(graphicsDevice);
        polygonBatch = std::make_unique<PolygonBatch>(graphicsContext, graphicsDevice, *assets);
    }
    {
        gameEditor = std::make_unique<SceneEditor::InGameEditor>(gameHost);
        editorBackground = std::make_unique<SceneEditor::EditorBackground>(gameHost);
    }

    {
        mainCamera = gameWorld.CreateObject();
        mainCamera.AddComponent<Transform2D>();
        mainCamera.AddComponent<Camera2D>();
    }
    {
        auto textureAtlas = TexturePacker::TextureAtlasLoader::Load(*assets, "MaidChan2/skeleton.atlas");
        auto skeletonDesc = Spine::SkeletonDescLoader::Load(*assets, "MaidChan2/skeleton.json");
        maidTexture = assets->Load<Texture2D>("MaidChan2/skeleton.png");

        LogTexturePackerInfo(textureAtlas);
        LogSkeletalInfo(skeletonDesc);

        maidSkeleton = std::make_shared<Skeleton>(Spine::CreateSkeleton(skeletonDesc.Bones));
        maidSkeletonPose = std::make_shared<SkeletonPose>(SkeletonPose::CreateBindPose(*maidSkeleton));
        auto animationClip = std::make_shared<AnimationClip>(Spine::CreateAnimationClip(skeletonDesc, "Walk"));
        maidAnimationState = std::make_shared<AnimationState>(animationClip, 1.0f, true);
        maidAnimationClipIdle = std::make_shared<AnimationClip>(Spine::CreateAnimationClip(skeletonDesc, "Idle"));

        maidSkin = Spine::CreateSkin(skeletonDesc, textureAtlas, "default");
        maidSpriteAnimationTracks = Spine::CreateSpriteAnimationTrack(skeletonDesc, textureAtlas, "Walk");

        animationSystem.Add(maidAnimationState, maidSkeleton, maidSkeletonPose);

        maidGlobalPose = SkeletonHelper::ToGlobalPose(*maidSkeleton, *maidSkeletonPose);

        // NOTE: for Skinning
        auto bindPose = SkeletonPose::CreateBindPose(*maidSkeleton);
        maidSkinnedMesh = Spine::CreateSkinnedMesh(*graphicsDevice,
            SkeletonHelper::ToGlobalPose(*maidSkeleton, bindPose),
            skeletonDesc, textureAtlas,
            Vector2(maidTexture->Width(), maidTexture->Height()), "default");
        maidSkinningEffect = std::make_unique<SkinnedEffect>(*graphicsDevice, *assets);
    }

    {
        scenePanel = std::make_shared<UI::ScenePanel>(clientBounds.Width, clientBounds.Height);
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

    clientViewport = Viewport{0, 0, clientBounds.Width, clientBounds.Height};

    connections.Connect(window->ClientSizeChanged, [this](int width, int height) {
        clientViewport = Viewport{0, 0, width, height};

        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice, width, height,
            false, SurfaceFormat::R8G8B8A8_UNorm, DepthFormat::None);

        fxaa->SetViewport(width, height);
        spriteRenderer->SetProjectionMatrix(Matrix4x4::CreateOrthographicLH(width, height, 1.0f, 100.0f));
    });
}

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
//            ///@note Test code for animation blending
//            auto clipNode1 = std::make_unique<AnimationClipNode>(maidAnimationState->Clip());
//            auto clipNode2 = std::make_unique<AnimationClipNode>(maidAnimationClipIdle);
//
//            auto lerpNode = std::make_unique<AnimationLerpNode>(std::move(clipNode1), std::move(clipNode2));
//            lerpNode->Weight(slider2->Value());
//
//            lerpNode->Calculate(maidAnimationState->Time(), *maidSkeleton, *maidSkeletonPose);

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

void GrassBlendingGame::DrawSprites()
{
    auto transform = mainCamera.Component<Transform2D>();
    auto camera = mainCamera.Component<Camera2D>();
    auto clientBounds = window->GetClientBounds();

    POMDOG_ASSERT(transform && camera);
    auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        clientBounds.Width, clientBounds.Height, 0.1f, 1000.0f);

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

void GrassBlendingGame::DrawSkinnedMesh()
{
    {
        auto transform = mainCamera.Component<Transform2D>();
        auto camera = mainCamera.Component<Camera2D>();

        POMDOG_ASSERT(transform && camera);
        auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
        auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
            clientViewport.Width, clientViewport.Height, 0.1f, 1000.0f);

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
        graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        graphicsContext->DrawIndexed(maidSkinnedMesh.IndexBuffer,
            maidSkinnedMesh.IndexBuffer->IndexCount());
    }

    if (toggleSwitch4->IsOn())
    {
        maidSkinningEffect->SetTexture(texture);
        graphicsContext->SetVertexBuffer(maidSkinnedMesh.VertexBuffer);
        maidSkinningEffect->Apply(*graphicsContext);
        graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
        graphicsContext->DrawIndexed(maidSkinnedMesh.IndexBuffer,
            maidSkinnedMesh.IndexBuffer->IndexCount());
    }
}

void GrassBlendingGame::Draw()
{
    constexpr bool enableFxaa = true;

    if (enableFxaa) {
        auto bounds = renderTarget->Bounds();
        graphicsContext->SetRenderTarget(renderTarget);
        graphicsContext->SetViewport(Viewport{bounds});
        graphicsContext->SetScissorRectangle(bounds);
    }

    graphicsContext->Clear(Color::CornflowerBlue);
    {
        auto transform = mainCamera.Component<Transform2D>();
        auto camera = mainCamera.Component<Camera2D>();

        POMDOG_ASSERT(transform && camera);
        auto viewMatrix = SandboxHelper::CreateViewMatrix(*transform, *camera);
        auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
            clientViewport.Width, clientViewport.Height, 0.1f, 1000.0f);

        editorBackground->SetViewProjection(viewMatrix * projectionMatrix);
    }
    editorBackground->Draw(*graphicsContext);

    graphicsContext->SetSamplerState(0, samplerPoint);
    DrawSprites();
    DrawSkinnedMesh();

    if (enableFxaa) {
        graphicsContext->SetRenderTarget();
        graphicsContext->SetViewport(clientViewport);
        graphicsContext->SetScissorRectangle(clientViewport.GetBounds());
        graphicsContext->Clear(Color::CornflowerBlue);
        fxaa->SetTexture(renderTarget);
        fxaa->Apply(*graphicsContext);
        screenQuad->DrawQuad(*graphicsContext);
    }

    gameEditor->DrawGUI(*graphicsContext);
    commandQueue->Present();
}

} // namespace TestApp
