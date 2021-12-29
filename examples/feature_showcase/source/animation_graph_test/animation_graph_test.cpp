#include "animation_graph_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/spine/animation_graph_builder.h"
#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/experimental/spine/skeleton_desc_loader.h"
#include "pomdog/experimental/spine/skeleton_loader.h"
#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/experimental/spine/skinned_mesh_loader.h"
#include "pomdog/experimental/texture_packer/texture_atlas_loader.h"
#include <random>

namespace feature_showcase {

AnimationGraphTest::AnimationGraphTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> AnimationGraphTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::CreateNonPremultiplied(),
        std::nullopt,
        gpu::SamplerDescriptor::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    auto texturePath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.png");
    auto textureAtlasPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.atlas");
    auto skeletonJSONPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.json");
    auto animationGraphJSONPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/AnimationGraph.json");

    // NOTE: Load texture file for skeletal animation model
    std::tie(texture, err) = assets->Load<gpu::Texture2D>(texturePath);
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load texture atlas file for skeletal animation model
    TexturePacker::TextureAtlas textureAtlas;
    std::tie(textureAtlas, err) = TexturePacker::TextureAtlasLoader::Load(textureAtlasPath);
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture atlas");
    }

    // NOTE: Load skeletal animation data
    if (auto [desc, descErr] = spine::SkeletonDescLoader::Load(skeletonJSONPath); descErr != nullptr) {
        return errors::Wrap(std::move(descErr), "failed to load skeleton JSON file");
    }
    else {
        skeleton = std::make_shared<skeletal2d::Skeleton>(spine::CreateSkeleton(desc.Bones));

        // NOTE: Create bind pose
        skeletonPose = std::make_shared<skeletal2d::SkeletonPose>(skeletal2d::SkeletonPose::CreateBindPose(*skeleton));

        // NOTE: Initialize global pose
        globalPose = skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose);

#if 1
        // FIXME: Isn't a bind pose required?
        for (auto& m : globalPose) {
            m = Matrix3x2::Identity();
        }
#endif

        // NOTE: Create skinned mesh
        auto textureSize = Vector2{static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight())};
        auto [skinnedMeshData, skinnedMeshErr] = spine::CreateSkinnedMesh(
            globalPose,
            desc,
            textureAtlas,
            textureSize,
            "default");
        if (skinnedMeshErr != nullptr) {
            return errors::Wrap(std::move(skinnedMeshErr), "failed to create skinned mesh data");
        }
        skinnedMesh = std::move(skinnedMeshData);

        // NOTE: Create animation graph
        auto [animationGraph, graphErr] = spine::LoadAnimationGraph(desc, animationGraphJSONPath);
        if (graphErr != nullptr) {
            return errors::Wrap(std::move(graphErr), "failed to create animation graph");
        }

        // NOTE: Create new skeleton animator
        animator = std::make_shared<skeletal2d::Animator>(skeleton, skeletonPose, animationGraph);

        animator->SetFloat("Run.Weight", 0.0f);
    }
    {
        using VertexCombined = BasicEffect::VertexPositionTexture;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 4> verticesCombo = {{
            {Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
        }};

        // NOTE: Create vertex buffer
        std::tie(vertexBuffer, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            skinnedMesh.Vertices.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
            gpu::IndexElementSize::SixteenBits,
            skinnedMesh.Indices.data(),
            skinnedMesh.Indices.size(),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(modelConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = false;
        effectDesc.TextureEnabled = true;
        effectDesc.VertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetDepthStencilState(gpu::DepthStencilDescriptor::CreateDefault())
            .SetBlendState(gpu::BlendDescriptor::CreateNonPremultiplied())
            .SetRasterizerState(gpu::RasterizerDescriptor::CreateDefault())
            .Build();
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create pipeline state");
        }

        // NOTE: Create pipeline state
        std::tie(pipelineStateWireframe, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetDepthStencilState(gpu::DepthStencilDescriptor::CreateDefault())
            .SetBlendState(gpu::BlendDescriptor::CreateOpaque())
            .SetRasterizerState(gpu::RasterizerDescriptor::CreateCullNoneWireframe())
            .Build();
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
            gpu::SamplerDescriptor::CreateLinearWrap());
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sampler state");
        }
    }

    return nullptr;
}

void AnimationGraphTest::Update()
{
    auto clock = gameHost->GetClock();

    auto mouse = gameHost->GetMouse()->GetState();
    if (mouse.LeftButton == ButtonState::Pressed) {
        auto window = gameHost->GetWindow();
        auto y = mouse.Position.Y - (window->GetClientBounds().Height / 2);
        auto blendWeight = std::clamp(static_cast<float>(y) / 180.0f, 0.0f, 1.0f);
        animator->SetFloat("Run.Weight", blendWeight);

        auto x = mouse.Position.X - (window->GetClientBounds().Width / 2);
        auto playbackRate = std::clamp(static_cast<float>(x) / 100.0f, -2.0f, 2.0f);
        animator->SetPlaybackRate(playbackRate);
    }

    // NOTE: (1) Pose extraction, and (2) Pose blending
    animator->Update(clock->GetFrameDuration());

    // NOTE: (3) Global pose generation
    skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose, globalPose);

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        -1.0f,
        1000.0f);

    auto viewMatrix = Matrix4x4::Identity();

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.InverseView = math::Invert(viewMatrix);
    worldConstants.LightDirection = Vector4{Vector3::UnitZ(), 0.0f};
    worldConstantBuffer->SetValue(worldConstants);

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.Model = Matrix4x4::CreateTranslation(Vector3{0.0f, -180.0f, 0.0f});
    modelConstants.Material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.Color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->SetValue(modelConstants);

    std::vector<BasicEffect::VertexPositionTexture> vertices;
    vertices.reserve(skinnedMesh.Vertices.size());

    for (const auto& skinVertex : skinnedMesh.Vertices) {
        using Vertex = BasicEffect::VertexPositionTexture;

        auto skinning = Matrix3x2{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        for (int i = 0; i < 4; i++) {
            const auto jointIndex = skinVertex.Joints[i];
            if (jointIndex < 0) {
                break;
            }

            const auto& boneMatrix = globalPose[jointIndex];
            skinning += boneMatrix * skinVertex.Weights[i];
        }

#if 1
        // FIXME: Ignore weighted blending.
        skinning = globalPose[skinVertex.Joints.front()];
#endif

        auto skinPos = Vector2{skinVertex.PositionTextureCoord.X, skinVertex.PositionTextureCoord.Y};
        auto position = math::Transform(skinPos, skinning);

        Vertex vertex;
        vertex.Position = Vector3{position.X, position.Y, 0.0f};
        vertex.TextureCoordinate = Vector2{skinVertex.PositionTextureCoord.Z, skinVertex.PositionTextureCoord.W};
        vertices.push_back(vertex);
    }

    vertexBuffer->SetData(vertices.data(), vertices.size());
}

void AnimationGraphTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    gpu::RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->End();

    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, modelConstantBuffer);
    commandList->SetConstantBuffer(1, worldConstantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetIndexBuffer(indexBuffer);
    commandList->DrawIndexed(indexBuffer->GetIndexCount(), 0);

    auto mouse = gameHost->GetMouse()->GetState();
    if (mouse.RightButton == ButtonState::Pressed) {
        commandList->SetPipelineState(pipelineStateWireframe);
        commandList->DrawIndexed(indexBuffer->GetIndexCount(), 0);
    }

    commandList->Close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->Reset();
        commandQueue->PushbackCommandList(commandList);
        commandQueue->ExecuteCommandLists();
        commandQueue->Present();
    }
    else {
        commandQueue->PushbackCommandList(commandList);
    }
}

} // namespace feature_showcase
