#include "skeletal2d_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/spine/animation_loader.h"
#include "pomdog/experimental/spine/skeleton_desc_loader.h"
#include "pomdog/experimental/spine/skeleton_loader.h"
#include "pomdog/experimental/spine/skin_loader.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

Skeletal2DTest::Skeletal2DTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
Skeletal2DTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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

    // NOTE: Load texture atlas file for skeletal animation model
    auto [textureAtlas, textureAtlasErr] = createTextureAtlas(
        fs_,
        graphicsDevice_,
        computeStringHash64("/assets"),
        computeStringHash64("skeletal2d/MaidChan/skeleton.tileset"),
        computeStringHash64("skeletal2d/MaidChan/skeleton.png"));
    if (textureAtlasErr != nullptr) {
        return errors::wrap(std::move(textureAtlasErr), "failed to load texture atlas");
    }
    texture_ = textureAtlas->getTexture();

    constexpr auto skeletonJSONPath = "/assets/skeletal2d/MaidChan/skeleton.json";

    // NOTE: Load skeletal animation data
    if (auto [desc, descErr] = spine::loadSkeletonDesc(fs_, skeletonJSONPath); descErr != nullptr) {
        return errors::wrap(std::move(descErr), "failed to load skeleton JSON file");
    }
    else {
        skeleton_ = std::make_shared<skeletal2d::Skeleton>(spine::CreateSkeleton(desc.Bones));

        // NOTE: Create bind pose
        skeletonPose_ = std::make_shared<skeletal2d::SkeletonPose>(skeletal2d::SkeletonPose::CreateBindPose(*skeleton_));

        // NOTE: Create animation clip and animation state
        auto [animationClip, clipErr] = spine::CreateAnimationClip(desc, textureAtlas, "Walk");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(clipErr), "failed to create animation clip");
        }
        animationState_ = std::make_shared<skeletal2d::AnimationState>(animationClip, 1.0f, true);

        // NOTE: Initialize global pose
        globalPose_ = skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton_, *skeletonPose_);

        // NOTE: Create skin
        skin_ = spine::CreateSkin(desc, *textureAtlas, "default");

        // NOTE: Add new skeleton animation to animation system
        animationSystem_.Add(animationState_, skeleton_, skeletonPose_, skin_);
    }

    {
        using VertexCombined = BasicEffect::VertexPositionTexture;

        // NOTE: Create vertex buffer
        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                static_cast<u32>(4 * skin_->GetSlots().size()),
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
        std::array<std::uint16_t, 6> quadIndices = {{0, 1, 2, 2, 3, 0}};

        std::vector<std::uint16_t> indices;
        indices.reserve(6 * skin_->GetSlots().size());

        for (std::size_t k = 0; k < skin_->GetSlots().size(); k++) {
            for (auto i : quadIndices) {
                indices.push_back(i + static_cast<std::uint16_t>(k * 4));
            }
        }

        if (auto [indexBuffer, err] = graphicsDevice_->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                indices.data(),
                static_cast<u32>(indices.size()),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer_ = std::move(indexBuffer);
        }
        wireframeIndices_ = std::move(indices);
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
            pipelineDesc.blendState = gpu::BlendDesc::createAlphaBlend();
            pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();

            // NOTE: Create pipeline state
            if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
                return errors::wrap(std::move(err), "failed to create pipeline state");
            }
            else {
                pipelineState_ = std::move(pipelineState);
            }
        }
        if (auto err = wireframeEffect_.initialize(fs_, graphicsDevice_); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize wireframe effect");
        }
    }
    {
        // NOTE: Create sampler state
        if (auto [sampler, err] = graphicsDevice_->createSamplerState(
                gpu::SamplerDesc::createLinearClamp());
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            sampler_ = std::move(sampler);
        }
    }

    return nullptr;
}

void Skeletal2DTest::update()
{
    auto clock = gameHost_->getClock();
    animationSystem_.Update(*clock);

    // NOTE: Global pose generation
    skeletal2d::SkeletonHelper::ToGlobalPose(*skeleton_, *skeletonPose_, globalPose_);

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
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
    for (const auto& slot : skin_->GetSlots()) {
        const auto slotCount = static_cast<int>(skin_->GetSlots().size());
        auto layerDepth = static_cast<float>(slotCount - slot.DrawOrder) / slotCount;
        auto poseMatrix = globalPose_[*slot.JointIndex];

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

        auto inverseTextureSize = Vector2{1.0f / static_cast<float>(texture_->getWidth()), 1.0f / static_cast<float>(texture_->getHeight())};
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
            auto pos = Vector2{v.position.x, v.position.y};
            pos = math::transform(pos - slot.Origin, transformMatrix);

            auto vertex = v;
            vertex.position = Vector3{pos, layerDepth};
            vertex.textureCoordinate = ((v.textureCoordinate * subrectSize) + subrectPos) * inverseTextureSize;
            vertices.push_back(vertex);
        }
    }

    vertexBuffer_->setData(vertices.data(), static_cast<u32>(vertices.size()));

    // NOTE: Update wireframe line list from the current animated quad positions.
    wireframeEffect_.setTriangleMesh(
        graphicsDevice_,
        Color{0, 220, 80, 200},
        static_cast<u32>(wireframeIndices_.size()),
        [&](u32 index) -> Vector3 {
            return vertices[wireframeIndices_[index]].position;
        });
}

void Skeletal2DTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);
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
        wireframeEffect_.draw(commandList_, modelConstantBuffer_, worldConstantBuffer_);
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
