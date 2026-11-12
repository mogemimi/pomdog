#include "mesh3d_test.h"
#include "pomdog/experimental/mesh3d/mesh_buffers.h"
#include "pomdog/experimental/mesh3d/mesh_data.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/vfs/file.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstring>
#include <span>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

[[nodiscard]] std::tuple<std::span<const u8>, std::shared_ptr<vfs::File>, std::unique_ptr<Error>>
viewFile(const std::shared_ptr<vfs::FileSystemContext>& fs, std::string_view path)
{
    auto [file, openErr] = vfs::open(fs, std::string{path});
    if (openErr != nullptr) {
        return {std::span<const u8>{}, nullptr, errors::wrap(std::move(openErr), "failed to open " + std::string{path})};
    }
    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {std::span<const u8>{}, nullptr, errors::wrap(std::move(viewErr), "failed to read " + std::string{path})};
    }
    return {bytes, std::move(file), nullptr};
}

} // namespace

Mesh3DTest::Mesh3DTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::tuple<std::shared_ptr<gpu::PipelineState>, std::unique_ptr<Error>>
Mesh3DTest::createPipelineState(BasicEffect::BasicEffectVariant variant)
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto [pipelineDesc, basicEffectErr] = BasicEffect::createBasicEffect(fs_, graphicsDevice_, variant);
    if (basicEffectErr != nullptr) {
        return {nullptr, errors::wrap(std::move(basicEffectErr), "failed to create basic effect")};
    }
    pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
    pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
    pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
    pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();
    pipelineDesc.blendState = gpu::BlendDesc::createNonPremultiplied();
    pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();

    // NOTE: The depth bias pushes the faces back a little, so that a wireframe
    // drawn over them passes the depth test.
    pipelineDesc.rasterizerState.depthBias = 1;
    pipelineDesc.rasterizerState.slopeScaledDepthBias = 1.0f;

    auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc);
    if (err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "failed to create pipeline state")};
    }
    return {std::move(pipelineState), nullptr};
}

std::unique_ptr<Error>
Mesh3DTest::loadMesh(const MeshDesc& desc, LoadedMesh& mesh)
{
    // NOTE: Load the meshes that the asset pipeline converted. The mesh file
    // stays in memory, and the vertices and the indices go to the GPU as they
    // are.
    auto [meshData, meshDataErr] = mesh3d::loadMeshData(fs_, desc.meshFilePath);
    if (meshDataErr != nullptr) {
        return errors::wrap(std::move(meshDataErr), "failed to load mesh file");
    }

    const auto meshIndex = meshData->findMeshByKey(computeStringHash32(desc.meshName));
    if (!meshIndex.has_value()) {
        return errors::make(pomdog::format("cannot find the mesh '{}' in {}", desc.meshName, desc.meshFilePath));
    }

    mesh.parts.clear();
    for (u32 i = 0; i < meshData->getPartCount(*meshIndex); ++i) {
        mesh.parts.push_back(meshData->getPart(*meshIndex, i));
    }
    if (mesh.parts.empty()) {
        return errors::make(pomdog::format("the mesh '{}' has no parts", desc.meshName));
    }

    // NOTE: This example draws every part with one vertex buffer.
    const auto vertexBufferIndex = mesh.parts.front().vertexBuffer;
    for (const auto& part : mesh.parts) {
        if (part.vertexBuffer != vertexBufferIndex) {
            return errors::make(pomdog::format("the parts of the mesh '{}' use different vertex buffers", desc.meshName));
        }
    }

    // NOTE: BasicEffect takes positions, normals, and then texture coordinates
    // or colors, so the vertices of the file must have the same layout.
    mesh.textured = (desc.variant == BasicEffect::BasicEffectVariant::PositionNormalTexture);
    const auto layout = meshData->getVertexBufferLayout(vertexBufferIndex);
    {
        const auto matches = [&](u32 elementIndex, mesh3d::VertexSemantic semantic, gpu::InputElementFormat format) -> bool {
            const auto element = meshData->getVertexElement(vertexBufferIndex, elementIndex);
            return (element.semantic == semantic) && (element.semanticIndex == 0) && (element.format == format);
        };
        const auto stride = mesh.textured ? sizeof(BasicEffect::VertexPositionNormalTexture) : sizeof(BasicEffect::VertexPositionNormalColor);
        const bool lastMatches = mesh.textured
                                     ? matches(2, mesh3d::VertexSemantic::TexCoord, gpu::InputElementFormat::Float32x2)
                                     : matches(2, mesh3d::VertexSemantic::Color, gpu::InputElementFormat::Float32x4);
        if ((layout.strideBytes != stride) ||
            (layout.elementCount != 3) ||
            !matches(0, mesh3d::VertexSemantic::Position, gpu::InputElementFormat::Float32x3) ||
            !matches(1, mesh3d::VertexSemantic::Normal, gpu::InputElementFormat::Float32x3) ||
            !lastMatches) {
            return errors::make(pomdog::format("the vertices of the mesh '{}' do not match the basic effect", desc.meshName));
        }
    }

    {
        auto [meshBuffers, err] = mesh3d::loadMeshBuffers(
            fs_,
            graphicsDevice_,
            *meshData,
            desc.verticesFilePath,
            desc.indicesFilePath);
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to load mesh buffers");
        }
        mesh.vertexBuffer = meshBuffers.vertexBuffers[vertexBufferIndex];
        mesh.indexBuffer = std::move(meshBuffers.indexBuffer);
    }

    // NOTE: Read the positions and the normals of the vertices from the
    // vertices file, which has no header; the mesh file tells where they are.
    // Each vertex starts with its position and its normal.
    std::vector<Vector3> positions(layout.vertexCount);
    std::vector<BasicEffect::VertexPositionColor> normalVertices(layout.vertexCount);
    {
        auto [bytes, file, err] = viewFile(fs_, desc.verticesFilePath);
        if (err != nullptr) {
            return std::move(err);
        }
        if (bytes.size() != meshData->getVerticesByteLength()) {
            return errors::make("the vertices file does not match the mesh file");
        }
        const auto vertices = bytes.subspan(layout.byteOffset, static_cast<std::size_t>(layout.vertexCount) * layout.strideBytes);
        for (u32 i = 0; i < layout.vertexCount; i++) {
            const auto vertex = vertices.subspan(static_cast<std::size_t>(i) * layout.strideBytes, layout.strideBytes);
            Vector3 normal;
            std::memcpy(&positions[i], vertex.data(), sizeof(Vector3));
            std::memcpy(&normal, vertex.subspan(sizeof(Vector3)).data(), sizeof(Vector3));
            normalVertices[i].position = positions[i];
            normalVertices[i].color = Vector4{normal, 1.0f};
        }
    }
    if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
            normalVertices.data(),
            static_cast<u32>(normalVertices.size()),
            sizeof(BasicEffect::VertexPositionColor),
            gpu::BufferUsage::Immutable);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create vertex buffer");
    }
    else {
        mesh.normalVertexBuffer = std::move(vertexBuffer);
    }

    // NOTE: Build the wireframe from the triangles of the parts.
    std::vector<Vector3> trianglePositions;
    {
        auto [bytes, file, err] = viewFile(fs_, desc.indicesFilePath);
        if (err != nullptr) {
            return std::move(err);
        }
        if (bytes.size() != meshData->getIndicesByteLength()) {
            return errors::make("the indices file does not match the mesh file");
        }
        const bool uses32Bit = (meshData->getIndexFormat() == gpu::IndexFormat::UInt32);
        const auto readIndex = [&](u32 i) -> u32 {
            if (uses32Bit) {
                u32 index = 0;
                std::memcpy(&index, bytes.subspan(static_cast<std::size_t>(i) * 4, 4).data(), 4);
                return index;
            }
            u16 index = 0;
            std::memcpy(&index, bytes.subspan(static_cast<std::size_t>(i) * 2, 2).data(), 2);
            return index;
        };
        for (const auto& part : mesh.parts) {
            for (u32 i = part.startIndex; i < part.startIndex + part.indexCount; ++i) {
                const auto index = readIndex(i);
                if (index >= positions.size()) {
                    return errors::make(pomdog::format("index {} of the mesh '{}' is out of range", index, desc.meshName));
                }
                trianglePositions.push_back(positions[index]);
            }
        }
    }
    // NOTE: The wireframe alone is bright on the background, and the wireframe
    // over the faces is dark on them.
    const std::array<std::pair<WireframeEffect*, Color>, 2> wireframes = {{
        {&mesh.wireframeEffect, Color{0, 220, 80, 255}},
        {&mesh.overlayWireframeEffect, Color{20, 20, 20, 255}},
    }};
    for (const auto& [effect, color] : wireframes) {
        if (auto err = effect->initialize(fs_, graphicsDevice_); err != nullptr) {
            return errors::wrap(std::move(err), "failed to initialize wireframe effect");
        }
        effect->setTriangleMesh(
            graphicsDevice_,
            color,
            static_cast<u32>(trianglePositions.size()),
            [&](u32 index) -> Vector3 { return trianglePositions[index]; });
    }

    if (auto [pipelineState, err] = createPipelineState(desc.variant); err != nullptr) {
        return std::move(err);
    }
    else {
        mesh.pipelineState = std::move(pipelineState);
    }

    mesh.baseMatrix = desc.baseMatrix;
    mesh.vertexCount = layout.vertexCount;
    mesh.triangleCount = static_cast<u32>(trianglePositions.size() / 3);
    return nullptr;
}

std::unique_ptr<Error>
Mesh3DTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Load texture from image file
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    // NOTE: The mesh of the glTF file is a cube whose corner is at the origin,
    // and the mesh of the MagicaVoxel file is centered and measured in voxels.
    const std::array<MeshDesc, 2> descs = {{
        {
            .meshFilePath = "/assets/glb/f15.mesh3d",
            .verticesFilePath = "/assets/glb/f15.mesh3dvertices",
            .indicesFilePath = "/assets/glb/f15.mesh3dindices",
            .meshName = "Cube",
            .variant = BasicEffect::BasicEffectVariant::PositionNormalTexture,
            .baseMatrix = Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f}),
        },
        {
            .meshFilePath = "/assets/voxel_models/maidchan.mesh3d",
            .verticesFilePath = "/assets/voxel_models/maidchan.mesh3dvertices",
            .indicesFilePath = "/assets/voxel_models/maidchan.mesh3dindices",
            .meshName = "model0",
            .variant = BasicEffect::BasicEffectVariant::PositionNormalColor,
            .baseMatrix = Matrix4x4::createScale(1.0f / 4.0f),
        },
    }};
    meshes_.resize(descs.size());
    for (std::size_t i = 0; i < descs.size(); ++i) {
        if (auto err = loadMesh(descs[i], meshes_[i]); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("failed to load {}", descs[i].meshFilePath));
        }
    }

    if (auto [pipelineState, err] = createPipelineState(BasicEffect::BasicEffectVariant::PositionColor); err != nullptr) {
        return std::move(err);
    }
    else {
        normalPipelineState_ = std::move(pipelineState);
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

    // NOTE: GUI
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    return nullptr;
}

void Mesh3DTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 320.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "mesh3d"_key, "Mesh3D", options);
    if (!window.isOpen()) {
        return;
    }

    constexpr std::array<std::string_view, 2> meshLabels = {
        "f15.glb (glTF)",
        "maidchan.vox (MagicaVoxel)",
    };
    devgui::combo(ui, "mesh"_key, "Mesh", meshIndex_, meshLabels);

    constexpr std::array<std::string_view, 4> displayModeLabels = {
        "Solid",
        "Normals as Colors",
        "Solid + Wireframe",
        "Wireframe",
    };
    devgui::combo(ui, "displayMode"_key, "Display", displayMode_, displayModeLabels);

    devgui::separator(ui);
    devgui::checkbox(ui, "autoRotate"_key, "Auto Rotate", autoRotate_);
    devgui::sliderFloat(ui, "yaw"_key, "Yaw", yawDegrees_, -180.0f, 180.0f);
    devgui::sliderFloat(ui, "pitch"_key, "Pitch", pitchDegrees_, -90.0f, 90.0f);

    devgui::separator(ui);

    const auto& mesh = meshes_[static_cast<std::size_t>(meshIndex_)];
    devgui::labelText(ui, "Parts", pomdog::format("{}", mesh.parts.size()));
    devgui::labelText(ui, "Vertices", pomdog::format("{}", mesh.vertexCount));
    devgui::labelText(ui, "Triangles", pomdog::format("{}", mesh.triangleCount));
}

void Mesh3DTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        static_cast<f32>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 8.0f, -2.0f};
    auto cameraDirection = Vector3{0.0f, -1.0f, 1.0f};
    auto viewMatrix = Matrix4x4::createLookAtLH(cameraPosition, cameraPosition + cameraDirection, Vector3::createUnitY());

    auto lightDirection = math::normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.viewProjection = viewMatrix * projectionMatrix;
    worldConstants.view = viewMatrix;
    worldConstants.projection = projectionMatrix;
    worldConstants.inverseView = math::invert(viewMatrix);
    worldConstants.lightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer_->setData(0, gpu::makeByteSpan(worldConstants));

    if (autoRotate_) {
        constexpr f32 degreesPerSecond = 90.0f;
        const auto frameDuration = static_cast<f32>(gameHost_->getClock()->getFrameDuration().count());
        yawDegrees_ += degreesPerSecond * frameDuration;
        if (yawDegrees_ >= 180.0f) {
            yawDegrees_ -= 360.0f;
        }
    }

    constexpr f32 radiansPerDegree = math::Pi<f32> / 180.0f;
    const auto& mesh = meshes_[static_cast<std::size_t>(meshIndex_)];
    auto modelMatrix =
        mesh.baseMatrix *
        Matrix4x4::createRotationY(yawDegrees_ * radiansPerDegree) *
        Matrix4x4::createRotationX(pitchDegrees_ * radiansPerDegree) *
        Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr f32 metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = modelMatrix;
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer_->setData(0, gpu::makeByteSpan(modelConstants));
}

void Mesh3DTest::draw()
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

    auto& mesh = meshes_[static_cast<std::size_t>(meshIndex_)];

    const auto displayMode = static_cast<DisplayMode>(displayMode_);

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    if (displayMode != DisplayMode::Wireframe) {
        commandList_->setConstantBuffer(0, modelConstantBuffer_);
        commandList_->setConstantBuffer(1, worldConstantBuffer_);
        if (displayMode == DisplayMode::Normals) {
            commandList_->setVertexBuffer(0, mesh.normalVertexBuffer);
            commandList_->setPipelineState(normalPipelineState_);
        }
        else {
            if (mesh.textured) {
                commandList_->setSamplerState(0, sampler_);
                commandList_->setTexture(0, texture_);
            }
            commandList_->setVertexBuffer(0, mesh.vertexBuffer);
            commandList_->setPipelineState(mesh.pipelineState);
        }
        commandList_->setIndexBuffer(mesh.indexBuffer);
        for (const auto& part : mesh.parts) {
            commandList_->drawIndexed(part.indexCount, part.startIndex);
        }
    }
    if (displayMode == DisplayMode::Wireframe) {
        mesh.wireframeEffect.draw(commandList_, modelConstantBuffer_, worldConstantBuffer_);
    }
    else if (displayMode == DisplayMode::SolidWithWireframe) {
        mesh.overlayWireframeEffect.draw(commandList_, modelConstantBuffer_, worldConstantBuffer_);
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
