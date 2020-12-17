#include "AnimationGraphTest.hpp"
#include <Pomdog/Experimental/Graphics/BasicEffect.hpp>
#include <Pomdog/Experimental/Skeletal2D/AnimationClip.hpp>
#include <Pomdog/Experimental/Skeletal2D/SkeletonHelper.hpp>
#include <Pomdog/Experimental/Spine/AnimationGraphBuilder.hpp>
#include <Pomdog/Experimental/Spine/AnimationLoader.hpp>
#include <Pomdog/Experimental/Spine/SkeletonDescLoader.hpp>
#include <Pomdog/Experimental/Spine/SkeletonLoader.hpp>
#include <Pomdog/Experimental/Spine/SkinLoader.hpp>
#include <Pomdog/Experimental/Spine/SkinnedMeshLoader.hpp>
#include <Pomdog/Experimental/TexturePacker/TextureAtlasLoader.hpp>
#include <random>

namespace FeatureShowcase {

AnimationGraphTest::AnimationGraphTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> AnimationGraphTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        std::nullopt,
        SamplerDescription::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    auto texturePath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.png");
    auto textureAtlasPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.atlas");
    auto skeletonJSONPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/MaidGun.json");
    auto animationGraphJSONPath = PathHelper::Join(assets->GetContentDirectory(), "Skeletal2D/MaidGun/AnimationGraph.json");

    // NOTE: Load texture file for skeletal animation model
    std::tie(texture, err) = assets->Load<Texture2D>(texturePath);
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load texture atlas file for skeletal animation model
    TexturePacker::TextureAtlas textureAtlas;
    std::tie(textureAtlas, err) = TexturePacker::TextureAtlasLoader::Load(textureAtlasPath);
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load texture atlas");
    }

    // NOTE: Load skeletal animation data
    if (auto [desc, descErr] = Spine::SkeletonDescLoader::Load(skeletonJSONPath); descErr != nullptr) {
        return Errors::Wrap(std::move(descErr), "failed to load skeleton JSON file");
    }
    else {
        skeleton = std::make_shared<Skeletal2D::Skeleton>(Spine::CreateSkeleton(desc.Bones));

        // NOTE: Create bind pose
        skeletonPose = std::make_shared<Skeletal2D::SkeletonPose>(Skeletal2D::SkeletonPose::CreateBindPose(*skeleton));

        // NOTE: Initialize global pose
        globalPose = Skeletal2D::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose);

#if 1
        // FIXME: Isn't a bind pose required?
        for (auto& m : globalPose) {
            m = Matrix3x2::Identity;
        }
#endif

        // NOTE: Create skinned mesh
        auto textureSize = Vector2{static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight())};
        auto [skinnedMeshData, skinnedMeshErr] = Spine::CreateSkinnedMesh(
            globalPose,
            desc,
            textureAtlas,
            textureSize,
            "default");
        if (skinnedMeshErr != nullptr) {
            return Errors::Wrap(std::move(skinnedMeshErr), "failed to create skinned mesh data");
        }
        skinnedMesh = std::move(skinnedMeshData);

        // NOTE: Create animation graph
        auto [animationGraph, graphErr] = Spine::LoadAnimationGraph(desc, animationGraphJSONPath);
        if (graphErr != nullptr) {
            return Errors::Wrap(std::move(graphErr), "failed to create animation graph");
        }

        // NOTE: Create new skeleton animator
        animator = std::make_shared<Skeletal2D::Animator>(skeleton, skeletonPose, animationGraph);

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
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            skinnedMesh.Indices.data(),
            skinnedMesh.Indices.size(),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(modelConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create constant buffer");
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
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create pipeline state");
        }

        // NOTE: Create pipeline state for wireframe debug rendering
        auto rasterizerDesc = RasterizerDescription::CreateCullNone();
        rasterizerDesc.FillMode = FillMode::WireFrame;

        // NOTE: Create pipeline state
        std::tie(pipelineStateWireframe, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateOpaque())
            .SetRasterizerState(rasterizerDesc)
            .Build();
        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
            SamplerDescription::CreateLinearWrap());
        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create sampler state");
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
    Skeletal2D::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose, globalPose);

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        -1.0f,
        1000.0f);

    auto viewMatrix = Matrix4x4::Identity;

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.InverseView = Matrix4x4::Invert(viewMatrix);
    worldConstants.LightDirection = Vector4{Vector3::UnitZ, 0.0f};
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
        auto position = Vector2::Transform(skinPos, skinning);

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

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
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

} // namespace FeatureShowcase
