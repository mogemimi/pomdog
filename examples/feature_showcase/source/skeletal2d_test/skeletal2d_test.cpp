#include "skeletal2d_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/experimental/spine/skeleton_desc_loader.h"
#include "pomdog/experimental/spine/skeleton_loader.h"
#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/experimental/texture_packer/texture_atlas_loader.h"
#include <random>

namespace feature_showcase {

Skeletal2DTest::Skeletal2DTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> Skeletal2DTest::initialize()
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

    auto texturePath = filepaths::join(assets->GetContentDirectory(), "Skeletal2D/MaidChan/skeleton.png");
    auto textureAtlasPath = filepaths::join(assets->GetContentDirectory(), "Skeletal2D/MaidChan/skeleton.atlas");
    auto skeletonJSONPath = filepaths::join(assets->GetContentDirectory(), "Skeletal2D/MaidChan/skeleton.json");

    // NOTE: Load texture file for skeletal animation model
    std::tie(texture, err) = assets->Load<gpu::Texture2D>(texturePath);
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

        // NOTE: Create animation clip and animation state
        auto [animationClip, clipErr] = spine::CreateAnimationClip(desc, textureAtlas, "Walk");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(clipErr), "failed to create animation clip");
        }
        animationState = std::make_shared<skeletal2d::AnimationState>(animationClip, 1.0f, true);

        // NOTE: Initialize global pose
        globalPose = skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton, *skeletonPose);

        // NOTE: Create skin
        skin = spine::CreateSkin(desc, textureAtlas, "default");

        // NOTE: Add new skeleton animation to animation system
        animationSystem.Add(animationState, skeleton, skeletonPose, skin);
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

        std::tie(vertexBuffer, err) = graphicsDevice->createVertexBuffer(
            verticesCombo.data(),
            4 * skin->GetSlots().size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::array<std::uint16_t, 6> quadIndices = {{0, 1, 2, 2, 3, 0}};

        std::vector<std::uint16_t> indices;
        indices.reserve(6 * skin->GetSlots().size());

        for (std::size_t k = 0; k < skin->GetSlots().size(); k++) {
            for (auto i : quadIndices) {
                indices.push_back(i + static_cast<std::uint16_t>(k * 4));
            }
        }

        std::tie(indexBuffer, err) = graphicsDevice->createIndexBuffer(
            gpu::IndexFormat::UInt16,
            indices.data(),
            indices.size(),
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
        effectDesc.LightingEnabled = false;
        effectDesc.TextureEnabled = true;
        effectDesc.VertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
            .SetBlendState(gpu::BlendDescriptor::createNonPremultiplied())
            .SetRasterizerState(gpu::RasterizerDescriptor::createDefault())
            .Build();

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }

        // NOTE: Create pipeline state
        std::tie(pipelineStateWireframe, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
            .SetBlendState(gpu::BlendDescriptor::createOpaque())
            .SetRasterizerState(gpu::RasterizerDescriptor::createCullNoneWireframe())
            .Build();

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->createSamplerState(
            gpu::SamplerDescriptor::createLinearWrap());

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }

    return nullptr;
}

void Skeletal2DTest::update()
{
    auto clock = gameHost->getClock();
    animationSystem.Update(*clock);

    // NOTE: Global pose generation
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
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.InverseView = math::invert(viewMatrix);
    worldConstants.LightDirection = Vector4{Vector3::createUnitZ(), 0.0f};
    worldConstantBuffer->setData(0, gpu::makeByteSpan(worldConstants));

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.Model = Matrix4x4::createTranslation(Vector3{0.0f, -180.0f, 0.0f});
    modelConstants.Material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.Color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->setData(0, gpu::makeByteSpan(modelConstants));

    std::vector<BasicEffect::VertexPositionTexture> vertices;
    for (const auto& slot : skin->GetSlots()) {
        const auto slotCount = static_cast<int>(skin->GetSlots().size());
        auto layerDepth = static_cast<float>(slotCount - slot.DrawOrder) / slotCount;
        auto poseMatrix = globalPose[*slot.JointIndex];

        using Vertex = BasicEffect::VertexPositionTexture;

        std::array<Vertex, 4> quadVertices;
        if (slot.TextureRotate) {
            quadVertices = {{
                {Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
                {Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
                {Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
                {Vector3{1.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            }};
        }
        else {
            quadVertices = {{
                {Vector3{0.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
                {Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
                {Vector3{1.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
                {Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            }};
        }

        auto inverseTextureSize = Vector2{1.0f / static_cast<float>(texture->getWidth()), 1.0f / static_cast<float>(texture->getHeight())};
        auto subrectPos = Vector2{static_cast<float>(slot.Subrect.x), static_cast<float>(slot.Subrect.y)};
        auto subrectSize = Vector2{static_cast<float>(slot.Subrect.width), static_cast<float>(slot.Subrect.height)};
        auto scaling = Matrix3x2::createScale(subrectSize);

        auto translate = Matrix3x2::createTranslation(slot.Translate);
        auto rotate = Matrix3x2::createRotation(slot.Rotation);

        if (slot.TextureRotate) {
            rotate = rotate * Matrix3x2::createRotation(-math::PiOver2<float>);
        }

        auto transformMatrix = scaling * rotate * translate * poseMatrix;

        for (const auto& v : quadVertices) {
            auto pos = Vector2{v.Position.x, v.Position.y};
            pos = math::transform(pos - slot.Origin, transformMatrix);

            auto vertex = v;
            vertex.Position = Vector3{pos, layerDepth};
            vertex.TextureCoordinate = ((v.TextureCoordinate * subrectSize) + subrectPos) * inverseTextureSize;
            vertices.push_back(vertex);
        }
    }

    vertexBuffer->setData(vertices.data(), vertices.size());
}

void Skeletal2DTest::draw()
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
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->End();

    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, modelConstantBuffer);
    commandList->setConstantBuffer(1, worldConstantBuffer);
    commandList->setSamplerState(0, sampler);
    commandList->setTexture(0, texture);
    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexed(indexBuffer->getIndexCount(), 0);

    auto mouse = gameHost->getMouse()->GetState();
    if (mouse.RightButton == ButtonState::Pressed) {
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
