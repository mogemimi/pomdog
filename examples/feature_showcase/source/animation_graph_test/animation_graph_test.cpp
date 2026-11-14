#include "animation_graph_test.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/animation_clip_data.h"
#include "pomdog/experimental/skeletal2d/animation_graph_builder.h"
#include "pomdog/experimental/skeletal2d/animation_loader.h"
#include "pomdog/experimental/skeletal2d/skeleton_data.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_loader.h"
#include "pomdog/experimental/skeletal2d/skin_data.h"
#include "pomdog/experimental/skeletal2d/skin_loader.h"
#include "pomdog/experimental/skeletal2d/skinned_mesh_loader.h"
#include "pomdog/experimental/texture_atlas/texture_atlas.h"
#include "pomdog/graphics/basic_effect.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

/// ModelDesc tells where the files of a model are and how to show it.
struct ModelDesc final {
    /// The name of the model, which names its directory under "skeletal2d"
    /// and its files, such as "maidgun/maidgun.tileset".
    std::string_view name;

    /// The states of the animation graph.
    std::span<const std::string_view> states;

    /// The float parameter that blends two clips, or empty.
    std::string_view weightParameter;

    /// Tells what the blend parameter does.
    std::string_view weightDescription;

    /// Moves the origin of the model from the center of the window.
    f32 offsetX = 0.0f;
    f32 offsetY = 0.0f;
};

constexpr std::array<std::string_view, 2> maidgunStates = {"Run", "Idle"};
constexpr std::array<std::string_view, 1> ghostStates = {"Hover"};
constexpr std::array<std::string_view, 2> majokkoStates = {"Run", "Idle"};

constexpr std::array<ModelDesc, 3> models = {{
    {
        .name = "maidgun",
        .states = maidgunStates,
        .weightParameter = "Run.Weight",
        .weightDescription = "Blends Run (0) and RunNoShot (1).",
        .offsetX = 0.0f,
        .offsetY = -180.0f,
    },
    {
        .name = "ghost",
        .states = ghostStates,
        .weightParameter = {},
        .weightDescription = {},
        .offsetX = 0.0f,
        .offsetY = -40.0f,
    },
    {
        .name = "majokko",
        .states = majokkoStates,
        .weightParameter = "Weight",
        .weightDescription = "Blends GoBackward (0) and GoForward (1).",
        .offsetX = 0.0f,
        .offsetY = -150.0f,
    },
}};

} // namespace

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
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
        else {
            sampler_ = std::move(sampler);
        }
    }

    if (auto err = loadModel(modelIndex_); err != nullptr) {
        return err;
    }

    // NOTE: GUI
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    return nullptr;
}

std::unique_ptr<Error>
AnimationGraphTest::loadModel(i32 modelIndex)
{
    const auto& desc = models[static_cast<std::size_t>(modelIndex)];
    const auto directory = "skeletal2d/" + std::string{desc.name} + "/";
    const auto filePath = [&](std::string_view fileName) {
        return directory + std::string{fileName};
    };
    const auto modelFilePath = [&](std::string_view extension) {
        return directory + std::string{desc.name} + std::string{extension};
    };

    // NOTE: Load texture atlas file for skeletal animation model
    auto [textureAtlas, textureAtlasErr] = createTextureAtlas(
        fs_,
        graphicsDevice_,
        computeStringHash64("/assets"),
        computeStringHash64(modelFilePath(".tileset")),
        computeStringHash64(modelFilePath(".png")));
    if (textureAtlasErr != nullptr) {
        return errors::wrap(std::move(textureAtlasErr), "failed to load texture atlas");
    }
    auto texture = textureAtlas->getTexture();

    // NOTE: Load skeletal animation data
    auto [skeletonData, skeletonDataErr] = skeletal2d::loadSkeletonData(fs_, "/assets/" + modelFilePath(".skeleton_skel2d"));
    if (skeletonDataErr != nullptr) {
        return errors::wrap(std::move(skeletonDataErr), "failed to load skeleton file");
    }
    auto [skinData, skinDataErr] = skeletal2d::loadSkinData(fs_, "/assets/" + modelFilePath(".skin_skel2d"));
    if (skinDataErr != nullptr) {
        return errors::wrap(std::move(skinDataErr), "failed to load skin file");
    }
    auto [clipData, clipDataErr] = skeletal2d::loadAnimationClipData(fs_, "/assets/" + modelFilePath(".clip_skel2d"));
    if (clipDataErr != nullptr) {
        return errors::wrap(std::move(clipDataErr), "failed to load clip file");
    }

    auto skeleton = std::make_shared<skeletal2d::Skeleton>(skeletal2d::createSkeleton(*skeletonData));

    // NOTE: Create bind pose
    auto skeletonPose = std::make_shared<skeletal2d::SkeletonPose>(skeletal2d::SkeletonPose::createBindPose(*skeleton));

    // NOTE: Initialize global pose
    auto globalPose = skeletal2d::SkeletonHelper::toGlobalPose(*skeleton, *skeletonPose);

#if 1
    // FIXME: Isn't a bind pose required?
    for (auto& m : globalPose) {
        m = Matrix3x2::createIdentity();
    }
#endif

    // NOTE: Create skinned mesh
    auto textureSize = Vector2{static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight())};
    auto [skinnedMesh, skinnedMeshErr] = skeletal2d::createSkinnedMesh(
        globalPose,
        *skinData,
        *textureAtlas,
        textureSize,
        "default");
    if (skinnedMeshErr != nullptr) {
        return errors::wrap(std::move(skinnedMeshErr), "failed to create skinned mesh data");
    }

    // NOTE: Create animation graph
    auto [animationGraph, graphErr] = skeletal2d::loadAnimationGraph(fs_, *clipData, "/assets/" + filePath("animgraph.animgraph_skel2d"));
    if (graphErr != nullptr) {
        return errors::wrap(std::move(graphErr), "failed to create animation graph");
    }

    // NOTE: Create vertex buffer
    using VertexCombined = BasicEffect::VertexPositionTexture;
    auto [vertexBuffer, vertexBufferErr] = graphicsDevice_->createVertexBuffer(
        skinnedMesh.vertices.data(),
        static_cast<u32>(skinnedMesh.vertices.size()),
        sizeof(VertexCombined),
        gpu::BufferUsage::Dynamic);
    if (vertexBufferErr != nullptr) {
        return errors::wrap(std::move(vertexBufferErr), "failed to create vertex buffer");
    }

    // NOTE: Create index buffer
    auto [indexBuffer, indexBufferErr] = graphicsDevice_->createIndexBuffer(
        gpu::IndexFormat::UInt16,
        skinnedMesh.indices.data(),
        static_cast<u32>(skinnedMesh.indices.size()),
        gpu::BufferUsage::Immutable);
    if (indexBufferErr != nullptr) {
        return errors::wrap(std::move(indexBufferErr), "failed to create index buffer");
    }

    // NOTE: Replace the model only after every part of it has loaded.
    texture_ = std::move(texture);
    skeleton_ = std::move(skeleton);
    skeletonPose_ = std::move(skeletonPose);
    globalPose_ = std::move(globalPose);
    skinnedMesh_ = std::move(skinnedMesh);
    vertexBuffer_ = std::move(vertexBuffer);
    indexBuffer_ = std::move(indexBuffer);

    // NOTE: Create new skeleton animator
    animator_ = std::make_shared<skeletal2d::Animator>(skeleton_, skeletonPose_, animationGraph);

    // NOTE: The panel shows the state that the animator starts with.
    stateIndex_ = 0;
    for (std::size_t i = 0; i < desc.states.size(); ++i) {
        if (computeStringHash32(desc.states[i]) == animator_->getCurrentStateKey()) {
            stateIndex_ = static_cast<i32>(i);
        }
    }
    applyParameters();
    return nullptr;
}

void AnimationGraphTest::applyParameters()
{
    const auto& desc = models[static_cast<std::size_t>(modelIndex_)];
    animator_->setPlaybackRate(playbackRate_);
    if (!desc.weightParameter.empty()) {
        animator_->setFloat(computeStringHash32(desc.weightParameter), weight_);
    }
}

void AnimationGraphTest::buildUI(devgui::UI& ui)
{
    using namespace pomdog::devgui::literals;

    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 260.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "animationGraph"_key, "Animation Graph", options);
    if (!window.isOpen()) {
        return;
    }

    std::array<std::string_view, models.size()> modelNames = {};
    for (std::size_t i = 0; i < models.size(); ++i) {
        modelNames[i] = models[i].name;
    }
    const auto previousModelIndex = modelIndex_;
    if (devgui::combo(ui, "model"_key, "Model", modelIndex_, modelNames).changed) {
        if (auto err = loadModel(modelIndex_); err != nullptr) {
            statusText_ = err->toString();
            modelIndex_ = previousModelIndex;
            return;
        }
        statusText_.clear();
    }

    const auto& desc = models[static_cast<std::size_t>(modelIndex_)];
    if (devgui::combo(ui, "state"_key, "State", stateIndex_, desc.states).changed) {
        constexpr auto transitionDuration = Duration{0.2};
        animator_->crossFade(computeStringHash32(desc.states[static_cast<std::size_t>(stateIndex_)]), transitionDuration);
    }
    if (devgui::sliderFloat(ui, "playbackRate"_key, "Playback Rate", playbackRate_, -2.0f, 2.0f).changed) {
        applyParameters();
    }
    if (!desc.weightParameter.empty()) {
        if (devgui::sliderFloat(ui, "weight"_key, desc.weightParameter, weight_, 0.0f, 1.0f).changed) {
            applyParameters();
        }
        devgui::text(ui, desc.weightDescription, ui.getStyle().textSecondaryColor);
    }
    devgui::checkbox(ui, "wireframe"_key, "Show Wireframe", showWireframe_);
    if (devgui::button(ui, "reset"_key, "Reset").clicked) {
        playbackRate_ = 1.0f;
        weight_ = 0.0f;
        applyParameters();
    }
    if (!statusText_.empty()) {
        devgui::text(ui, statusText_, ui.getStyle().textSecondaryColor);
    }
}

void AnimationGraphTest::update()
{
    auto clock = gameHost_->getClock();

    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });

    // NOTE: (1) Pose extraction, and (2) Pose blending
    animator_->update(clock->getFrameDuration());

    // NOTE: (3) Global pose generation
    skeletal2d::SkeletonHelper::toGlobalPose(*skeleton_, *skeletonPose_, globalPose_);

    // NOTE: The skinned mesh is positioned in logical pixels, so the
    // projection uses the logical client size (DPI-independent layout).
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
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
    const auto& desc = models[static_cast<std::size_t>(modelIndex_)];
    modelConstants.model = Matrix4x4::createTranslation(Vector3{desc.offsetX, desc.offsetY, 0.0f});
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer_->setData(0, gpu::makeByteSpan(modelConstants));

    std::vector<BasicEffect::VertexPositionTexture> vertices;
    vertices.reserve(skinnedMesh_.vertices.size());

    for (const auto& skinVertex : skinnedMesh_.vertices) {
        using Vertex = BasicEffect::VertexPositionTexture;

        auto skinning = Matrix3x2{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        for (int i = 0; i < 4; i++) {
            const auto jointIndex = skinVertex.joints[i];
            if (jointIndex < 0) {
                break;
            }

            const auto& boneMatrix = globalPose_[jointIndex];
            skinning += boneMatrix * skinVertex.weights[i];
        }

#if 1
        // FIXME: Ignore weighted blending.
        skinning = globalPose_[skinVertex.joints.front()];
#endif

        auto skinPos = Vector2{skinVertex.positionTextureCoord.x, skinVertex.positionTextureCoord.y};
        auto position = math::transform(skinPos, skinning);

        Vertex vertex;
        vertex.position = Vector3{position.x, position.y, 0.0f};
        vertex.textureCoordinate = Vector2{skinVertex.positionTextureCoord.z, skinVertex.positionTextureCoord.w};
        vertices.push_back(vertex);
    }

    vertexBuffer_->setData(vertices.data(), static_cast<u32>(vertices.size()));

    // NOTE: Update wireframe line list from the current skinned positions.
    wireframeEffect_.setTriangleMesh(
        graphicsDevice_,
        Color{0, 220, 80, 200},
        static_cast<u32>(skinnedMesh_.indices.size()),
        [&](u32 index) -> Vector3 {
            return vertices[skinnedMesh_.indices[index]].position;
        });
}

void AnimationGraphTest::draw()
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

    // NOTE: 2D content is positioned in logical pixels, so the projection and
    // grid lines use the logical client size. The viewport stays physical.
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<f32>(clientBounds.width);
    const auto h = static_cast<f32>(clientBounds.height);
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

    if (showWireframe_) {
        wireframeEffect_.draw(commandList_, modelConstantBuffer_, worldConstantBuffer_);
    }

    commandList_->endRenderPass();

    // NOTE: Draw the panel over the scene.
    {
        gpu::RenderPass overlayPass;
        overlayPass.renderTargets[0] = {nullptr, std::nullopt};
        overlayPass.viewport = viewport;
        overlayPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(overlayPass));
    }
    overlay_.draw(commandList_);

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
