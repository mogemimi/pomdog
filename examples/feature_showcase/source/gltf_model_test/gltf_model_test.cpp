#include "gltf_model_test.h"
#include "pomdog/experimental/mesh3d/mesh_buffers.h"
#include "pomdog/experimental/mesh3d/mesh_data.h"
#include "pomdog/graphics/basic_effect.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/vfs/file.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

GLTFModelTest::GLTFModelTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
GLTFModelTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

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

    // NOTE: Load the meshes that the asset pipeline converted from a glTF
    // binary file. The mesh file stays in memory, and the vertices and the
    // indices go to the GPU as they are.
    auto [meshData, meshDataErr] = mesh3d::loadMeshData(fs_, "/assets/glb/f15.mesh3d");
    if (meshDataErr != nullptr) {
        return errors::wrap(std::move(meshDataErr), "failed to load mesh file");
    }

    const auto meshIndex = meshData->findMeshByKey(computeStringHash32("Cube"));
    if (!meshIndex.has_value()) {
        return errors::make("cannot find the mesh 'Cube' in the mesh file");
    }

    meshParts_.clear();
    for (u32 i = 0; i < meshData->getPartCount(*meshIndex); ++i) {
        meshParts_.push_back(meshData->getPart(*meshIndex, i));
    }
    if (meshParts_.empty()) {
        return errors::make("the mesh 'Cube' has no parts");
    }

    // NOTE: This example draws every part with one vertex buffer.
    const auto vertexBufferIndex = meshParts_.front().vertexBuffer;
    for (const auto& part : meshParts_) {
        if (part.vertexBuffer != vertexBufferIndex) {
            return errors::make("the parts of the mesh 'Cube' use different vertex buffers");
        }
    }

    // NOTE: BasicEffect takes positions, normals, and texture coordinates in
    // this order, so the vertices of the file must have the same layout.
    using VertexCombined = BasicEffect::VertexPositionNormalTexture;
    const auto layout = meshData->getVertexBufferLayout(vertexBufferIndex);
    {
        const auto matches = [&](u32 elementIndex, mesh3d::VertexSemantic semantic, gpu::InputElementFormat format) -> bool {
            const auto element = meshData->getVertexElement(vertexBufferIndex, elementIndex);
            return (element.semantic == semantic) && (element.semanticIndex == 0) && (element.format == format);
        };
        if ((layout.strideBytes != sizeof(VertexCombined)) ||
            (layout.elementCount != 3) ||
            !matches(0, mesh3d::VertexSemantic::Position, gpu::InputElementFormat::Float32x3) ||
            !matches(1, mesh3d::VertexSemantic::Normal, gpu::InputElementFormat::Float32x3) ||
            !matches(2, mesh3d::VertexSemantic::TexCoord, gpu::InputElementFormat::Float32x2)) {
            return errors::make("the mesh 'Cube' must have positions, normals, and texture coordinates");
        }
    }

    constexpr auto verticesFilePath = "/assets/glb/f15.mesh3dvertices";
    {
        auto [meshBuffers, err] = mesh3d::loadMeshBuffers(
            fs_,
            graphicsDevice_,
            *meshData,
            verticesFilePath,
            "/assets/glb/f15.mesh3dindices");
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to load mesh buffers");
        }
        vertexBuffer1_ = meshBuffers.vertexBuffers[vertexBufferIndex];
        indexBuffer_ = std::move(meshBuffers.indexBuffer);
    }
    {
        // NOTE: Create a vertex buffer that shows the normals as colors. The
        // vertices file has no header, so the mesh file tells where the
        // vertices are.
        auto [file, openErr] = vfs::open(fs_, verticesFilePath);
        if (openErr != nullptr) {
            return errors::wrap(std::move(openErr), "failed to open vertices file");
        }
        auto [bytes, viewErr] = file->view();
        if (viewErr != nullptr) {
            return errors::wrap(std::move(viewErr), "failed to read vertices file");
        }
        if (bytes.size() != meshData->getVerticesByteLength()) {
            return errors::make("the vertices file does not match the mesh file");
        }

        std::vector<BasicEffect::VertexPositionColor> verticesCombo;
        verticesCombo.resize(layout.vertexCount);

        const auto vertices = bytes.subspan(layout.byteOffset, static_cast<std::size_t>(layout.vertexCount) * layout.strideBytes);
        for (u32 i = 0; i < layout.vertexCount; i++) {
            VertexCombined vertex;
            std::memcpy(&vertex, vertices.subspan(static_cast<std::size_t>(i) * layout.strideBytes, sizeof(vertex)).data(), sizeof(vertex));

            verticesCombo[i].position = vertex.position;
            verticesCombo[i].color = Vector4{vertex.normal, 1.0f};
        }

        if (auto [vertexBuffer2, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(BasicEffect::VertexPositionColor),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer2_ = std::move(vertexBuffer2);
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
    {
        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        BasicEffect::BasicEffectVariant variant = BasicEffect::BasicEffectVariant::PositionNormalTexture;

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
        if (auto [pipelineState1, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState1_ = std::move(pipelineState1);
        }
    }
    {
        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        BasicEffect::BasicEffectVariant variant = BasicEffect::BasicEffectVariant::PositionColor;

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
        if (auto [pipelineState2, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState2_ = std::move(pipelineState2);
        }
    }

    return nullptr;
}

void GLTFModelTest::update()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

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

    auto time = static_cast<f32>(gameHost_->getClock()->getTotalGameTime().count());
    auto rotateY = math::TwoPi<f32> * rotateSpeed * time;

    const auto mouse = gameHost_->getMouse();
    const auto gesture = getPrimaryGestureState(*mouse, gameHost_->getTouchscreen().get());
    if (gesture.pressed) {
        // NOTE: Normalize the pointer position by the logical client width so
        // the rotation tracks the mouse or touch regardless of DPI.
        const auto clientBounds = gameHost_->getWindow()->getClientBounds();
        rotateY = -math::TwoPi<f32> * (static_cast<f32>(gesture.position.x) / static_cast<f32>(clientBounds.width));
    }

    auto modelMatrix =
        Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f}) *
        Matrix4x4::createScale(1.0f + std::cos(time * 5.0f) * 0.1f) *
        Matrix4x4::createRotationY(rotateY) *
        Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr f32 metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = modelMatrix;
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer_->setData(0, gpu::makeByteSpan(modelConstants));
}

void GLTFModelTest::draw()
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

    const auto mouse = gameHost_->getMouse();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    commandList_->setConstantBuffer(0, modelConstantBuffer_);
    commandList_->setConstantBuffer(1, worldConstantBuffer_);
    commandList_->setSamplerState(0, sampler_);
    commandList_->setTexture(0, texture_);
    if (mouse->isButtonDown(MouseButtons::Right)) {
        commandList_->setVertexBuffer(0, vertexBuffer2_);
        commandList_->setPipelineState(pipelineState2_);
    }
    else {
        commandList_->setVertexBuffer(0, vertexBuffer1_);
        commandList_->setPipelineState(pipelineState1_);
    }
    commandList_->setIndexBuffer(indexBuffer_);
    for (const auto& part : meshParts_) {
        commandList_->drawIndexed(part.indexCount, part.startIndex);
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
