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

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

AnimationGraphTest::AnimationGraphTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> AnimationGraphTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::createNonPremultiplied(),
        std::nullopt,
        gpu::SamplerDescriptor::createPointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    auto texturePath = filepaths::join(assets->getContentDirectory(), "Skeletal2D/MaidGun/MaidGun.png");
    auto textureAtlasPath = filepaths::join(assets->getContentDirectory(), "Skeletal2D/MaidGun/MaidGun.atlas");
    auto skeletonJSONPath = filepaths::join(assets->getContentDirectory(), "Skeletal2D/MaidGun/MaidGun.json");
    auto animationGraphJSONPath = filepaths::join(assets->getContentDirectory(), "Skeletal2D/MaidGun/AnimationGraph.json");

    // NOTE: Load texture file for skeletal animation model
    std::tie(texture, err) = assets->load<gpu::Texture2D>(texturePath);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load texture atlas file for skeletal animation model
    TexturePacker::TextureAtlas textureAtlas;
    std::tie(textureAtlas, err) = TexturePacker::TextureAtlasLoader::Load(textureAtlasPath);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture atlas");
    }

    // NOTE: Load skeletal animation data
    if (auto [desc, descErr] = spine::SkeletonDescLoader::Load(skeletonJSONPath); descErr != nullptr) {
        return errors::wrap(std::move(descErr), "failed to load skeleton JSON file");
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
            m = Matrix3x2::createIdentity();
        }
#endif

        // NOTE: Create skinned mesh
        auto textureSize = Vector2{static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight())};
        auto [skinnedMeshData, skinnedMeshErr] = spine::CreateSkinnedMesh(
            globalPose,
            desc,
            textureAtlas,
            textureSize,
            "default");
        if (skinnedMeshErr != nullptr) {
            return errors::wrap(std::move(skinnedMeshErr), "failed to create skinned mesh data");
        }
        skinnedMesh = std::move(skinnedMeshData);

        // NOTE: Create animation graph
        auto [animationGraph, graphErr] = spine::LoadAnimationGraph(desc, animationGraphJSONPath);
        if (graphErr != nullptr) {
            return errors::wrap(std::move(graphErr), "failed to create animation graph");
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
        std::tie(vertexBuffer, err) = graphicsDevice->createVertexBuffer(
            verticesCombo.data(),
            skinnedMesh.Vertices.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::tie(indexBuffer, err) = graphicsDevice->createIndexBuffer(
            gpu::IndexFormat::UInt16,
            skinnedMesh.Indices.data(),
            skinnedMesh.Indices.size(),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(modelConstantBuffer, err) = graphicsDevice->createConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->createConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        auto presentationParameters = graphicsDevice->getPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.lightingEnabled = false;
        effectDesc.textureEnabled = true;
        effectDesc.vertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = BasicEffect::createBasicEffect(*assets, effectDesc)
            .setRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .setDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
            .setBlendState(gpu::BlendDescriptor::createNonPremultiplied())
            .setRasterizerState(gpu::RasterizerDescriptor::createDefault())
            .build();
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }

        // NOTE: Create pipeline state
        std::tie(pipelineStateWireframe, err) = BasicEffect::createBasicEffect(*assets, effectDesc)
            .setRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .setDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
            .setBlendState(gpu::BlendDescriptor::createOpaque())
            .setRasterizerState(gpu::RasterizerDescriptor::createCullNoneWireframe())
            .build();
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->createSamplerState(
            gpu::SamplerDescriptor::createLinearWrap());
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
    }

    return nullptr;
}

void AnimationGraphTest::update()
{
    auto clock = gameHost->getClock();

    const auto mouse = gameHost->getMouse()->getState();
    if (mouse.leftButton == ButtonState::Pressed) {
        const auto window = gameHost->getWindow();
        const auto clientBounds = window->getClientBounds();

        const auto y = mouse.position.y - (clientBounds.height / 2);
        const auto blendWeight = std::clamp(static_cast<float>(y) / 180.0f, 0.0f, 1.0f);
        animator->SetFloat("Run.Weight", blendWeight);

        const auto x = mouse.position.x - (clientBounds.width / 2);
        const auto playbackRate = std::clamp(static_cast<float>(x) / 100.0f, -2.0f, 2.0f);
        animator->SetPlaybackRate(playbackRate);
    }

    // NOTE: (1) Pose extraction, and (2) Pose blending
    animator->Update(clock->getFrameDuration());

    // NOTE: (3) Global pose generation
    skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose, globalPose);

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        -1.0f,
        1000.0f);

    auto viewMatrix = Matrix4x4::createIdentity();

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.viewProjection = viewMatrix * projectionMatrix;
    worldConstants.inverseView = math::invert(viewMatrix);
    worldConstants.lightDirection = Vector4{Vector3::createUnitZ(), 0.0f};
    worldConstantBuffer->setData(0, gpu::makeByteSpan(worldConstants));

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = Matrix4x4::createTranslation(Vector3{0.0f, -180.0f, 0.0f});
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->setData(0, gpu::makeByteSpan(modelConstants));

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

        auto skinPos = Vector2{skinVertex.PositionTextureCoord.x, skinVertex.PositionTextureCoord.y};
        auto position = math::transform(skinPos, skinning);

        Vertex vertex;
        vertex.position = Vector3{position.x, position.y, 0.0f};
        vertex.textureCoordinate = Vector2{skinVertex.PositionTextureCoord.z, skinVertex.PositionTextureCoord.w};
        vertices.push_back(vertex);
    }

    vertexBuffer->setData(vertices.data(), vertices.size());
}

void AnimationGraphTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList->reset();
    commandList->setRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->begin(commandList, projectionMatrix);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->end();

    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, modelConstantBuffer);
    commandList->setConstantBuffer(1, worldConstantBuffer);
    commandList->setSamplerState(0, sampler);
    commandList->setTexture(0, texture);
    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexed(indexBuffer->getIndexCount(), 0);

    const auto mouse = gameHost->getMouse()->getState();
    if (mouse.rightButton == ButtonState::Pressed) {
        commandList->setPipelineState(pipelineStateWireframe);
        commandList->drawIndexed(indexBuffer->getIndexCount(), 0);
    }

    commandList->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->reset();
        commandQueue->pushBackCommandList(commandList);
        commandQueue->executeCommandLists();
        commandQueue->present();
    }
    else {
        commandQueue->pushBackCommandList(commandList);
    }
}

} // namespace feature_showcase
