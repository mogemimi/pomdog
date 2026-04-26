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

AnimationGraphTest::AnimationGraphTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
AnimationGraphTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }

    const auto texturePath = "/assets/skeletal2d/MaidGun/MaidGun.png";
    const auto textureAtlasPath = "/assets/skeletal2d/MaidGun/MaidGun.atlas";
    const auto skeletonJSONPath = "/assets/skeletal2d/MaidGun/MaidGun.json";
    const auto animationGraphJSONPath = "/assets/skeletal2d/MaidGun/AnimationGraph.json";

    // NOTE: Load texture file for skeletal animation model
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, texturePath); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    // NOTE: Load texture atlas file for skeletal animation model
    auto [textureAtlas, textureAtlasErr] = TexturePacker::loadTextureAtlas(fs_, textureAtlasPath);
    if (textureAtlasErr != nullptr) {
        return errors::wrap(std::move(textureAtlasErr), "failed to load texture atlas");
    }

    // NOTE: Load skeletal animation data
    if (auto [desc, descErr] = spine::loadSkeletonDesc(fs_, skeletonJSONPath); descErr != nullptr) {
        return errors::wrap(std::move(descErr), "failed to load skeleton JSON file");
    }
    else {
        skeleton_ = std::make_shared<skeletal2d::Skeleton>(spine::CreateSkeleton(desc.Bones));

        // NOTE: Create bind pose
        skeletonPose_ = std::make_shared<skeletal2d::SkeletonPose>(skeletal2d::SkeletonPose::CreateBindPose(*skeleton_));

        // NOTE: Initialize global pose
        globalPose_ = skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton_, *skeletonPose_);

#if 1
        // FIXME: Isn't a bind pose required?
        for (auto& m : globalPose_) {
            m = Matrix3x2::createIdentity();
        }
#endif

        // NOTE: Create skinned mesh
        auto textureSize = Vector2{static_cast<float>(texture_->getWidth()), static_cast<float>(texture_->getHeight())};
        auto [skinnedMeshData, skinnedMeshErr] = spine::CreateSkinnedMesh(
            globalPose_,
            desc,
            textureAtlas,
            textureSize,
            "default");
        if (skinnedMeshErr != nullptr) {
            return errors::wrap(std::move(skinnedMeshErr), "failed to create skinned mesh data");
        }
        skinnedMesh_ = std::move(skinnedMeshData);

        // NOTE: Create animation graph
        auto [animationGraph, graphErr] = spine::loadAnimationGraph(fs_, desc, animationGraphJSONPath);
        if (graphErr != nullptr) {
            return errors::wrap(std::move(graphErr), "failed to create animation graph");
        }

        // NOTE: Create new skeleton animator
        animator_ = std::make_shared<skeletal2d::Animator>(skeleton_, skeletonPose_, animationGraph);

        animator_->SetFloat("Run.Weight", 0.0f);
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
        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(skinnedMesh_.Vertices.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(vertexBuffer);
        }
    }
    {
        // NOTE: Create index buffer
        if (auto [indexBuffer, err] = graphicsDevice_->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                skinnedMesh_.Indices.data(),
                static_cast<u32>(skinnedMesh_.Indices.size()),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer_ = std::move(indexBuffer);
        }
    }
    {
        // NOTE: Create constant buffer
        if (auto [modelConstantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(BasicEffect::ModelConstantBuffer),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            modelConstantBuffer_ = std::move(modelConstantBuffer);
        }

        if (auto [worldConstantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(BasicEffect::WorldConstantBuffer),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            worldConstantBuffer_ = std::move(worldConstantBuffer);
        }
    }
    {
        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        BasicEffect::BasicEffectVariant variant = BasicEffect::BasicEffectVariant::PositionTexture;

        {
            auto [pipelineDesc, basicEffectErr] = BasicEffect::createBasicEffect(fs_, graphicsDevice_, variant);
            if (basicEffectErr != nullptr) {
                return errors::wrap(std::move(basicEffectErr), "failed to create basic effect");
            }
            pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
            pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
            pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
            pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
            pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
            pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();

            // NOTE: Create pipeline state
            if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
                return errors::wrap(std::move(err), "failed to create pipeline state");
            }
            else {
                pipelineState_ = std::move(pipelineState);
            }
        }
        {
            auto [pipelineDesc, basicEffectErr] = BasicEffect::createBasicEffect(fs_, graphicsDevice_, variant);
            if (basicEffectErr != nullptr) {
                return errors::wrap(std::move(basicEffectErr), "failed to create basic effect");
            }
            pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
            pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
            pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
            pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
            pipelineDesc.blendState = gpu::BlendDesc::createOpaque();
            pipelineDesc.rasterizerState = gpu::RasterizerDesc::createCullNoneWireframe();

            // NOTE: Create pipeline state
            if (auto [pipelineStateWireframe, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
                return errors::wrap(std::move(err), "failed to create pipeline state");
            }
            else {
                pipelineStateWireframe_ = std::move(pipelineStateWireframe);
            }
        }
    }
    {
        // NOTE: Create sampler state
        if (auto [sampler, err] = graphicsDevice_->createSamplerState(
                gpu::SamplerDesc::createLinearClamp());
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
        else {
            sampler_ = std::move(sampler);
        }
    }

    return nullptr;
}

void AnimationGraphTest::update()
{
    auto clock = gameHost_->getClock();

    const auto mouse = gameHost_->getMouse();
    if (mouse->isButtonDown(MouseButtons::Left)) {
        const auto window = gameHost_->getWindow();
        const auto clientBounds = window->getClientBounds();

        const auto y = mouse->getPosition().y - (clientBounds.height / 2);
        const auto blendWeight = std::clamp(static_cast<float>(y) / 180.0f, 0.0f, 1.0f);
        animator_->SetFloat("Run.Weight", blendWeight);

        const auto x = mouse->getPosition().x - (clientBounds.width / 2);
        const auto playbackRate = std::clamp(static_cast<float>(x) / 100.0f, -2.0f, 2.0f);
        animator_->SetPlaybackRate(playbackRate);
    }

    // NOTE: (1) Pose extraction, and (2) Pose blending
    animator_->Update(clock->getFrameDuration());

    // NOTE: (3) Global pose generation
    skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton_, *skeletonPose_, globalPose_);

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

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
    worldConstantBuffer_->setData(0, gpu::makeByteSpan(worldConstants));

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = Matrix4x4::createTranslation(Vector3{0.0f, -180.0f, 0.0f});
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer_->setData(0, gpu::makeByteSpan(modelConstants));

    std::vector<BasicEffect::VertexPositionTexture> vertices;
    vertices.reserve(skinnedMesh_.Vertices.size());

    for (const auto& skinVertex : skinnedMesh_.Vertices) {
        using Vertex = BasicEffect::VertexPositionTexture;

        auto skinning = Matrix3x2{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        for (int i = 0; i < 4; i++) {
            const auto jointIndex = skinVertex.Joints[i];
            if (jointIndex < 0) {
                break;
            }

            const auto& boneMatrix = globalPose_[jointIndex];
            skinning += boneMatrix * skinVertex.Weights[i];
        }

#if 1
        // FIXME: Ignore weighted blending.
        skinning = globalPose_[skinVertex.Joints.front()];
#endif

        auto skinPos = Vector2{skinVertex.PositionTextureCoord.x, skinVertex.PositionTextureCoord.y};
        auto position = math::transform(skinPos, skinning);

        Vertex vertex;
        vertex.position = Vector3{position.x, position.y, 0.0f};
        vertex.textureCoordinate = Vector2{skinVertex.PositionTextureCoord.z, skinVertex.PositionTextureCoord.w};
        vertices.push_back(vertex);
    }

    vertexBuffer_->setData(vertices.data(), static_cast<u32>(vertices.size()));
}

void AnimationGraphTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    commandList_->setPipelineState(pipelineState_);
    commandList_->setConstantBuffer(0, modelConstantBuffer_);
    commandList_->setConstantBuffer(1, worldConstantBuffer_);
    commandList_->setSamplerState(0, sampler_);
    commandList_->setTexture(0, texture_);
    commandList_->setVertexBuffer(0, vertexBuffer_);
    commandList_->setIndexBuffer(indexBuffer_);
    commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);

    const auto mouse = gameHost_->getMouse();
    if (mouse->isButtonDown(MouseButtons::Right)) {
        commandList_->setPipelineState(pipelineStateWireframe_);
        commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);
    }

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
