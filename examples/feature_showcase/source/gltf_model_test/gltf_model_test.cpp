#include "gltf_model_test.h"
#include "pomdog/experimental/gltf/gltf.h"
#include "pomdog/experimental/gltf/gltf_loader.h"
#include "pomdog/experimental/graphics/basic_effect.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
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

    // NOTE: Load model from glTF binary file.
    auto [glb, glbErr] = GLTF::loadGLTF(fs_, "/assets/glb/f15.glb");
    if (glbErr != nullptr) {
        return errors::wrap(std::move(glbErr), "failed to load glTF binary");
    }

    POMDOG_ASSERT(!glb->Meshes.empty());
    POMDOG_ASSERT(!glb->Meshes[0].Primitives.empty());
    POMDOG_ASSERT(glb->Meshes[0].Primitives[0].Indices != std::nullopt);
    const auto positionAccessor = glb->Meshes[0].Primitives[0].Attributes["POSITION"];
    const auto normalAccessor = glb->Meshes[0].Primitives[0].Attributes["NORMAL"];
    const auto texcoordAccessor = glb->Meshes[0].Primitives[0].Attributes["TEXCOORD_0"];
    const auto indexAccessor = *glb->Meshes[0].Primitives[0].Indices;

    POMDOG_ASSERT(glb->Accessors.size() == 4);
    POMDOG_ASSERT(glb->Accessors[0].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[0].Type == GLTF::AccessorType::Vec3);
    POMDOG_ASSERT(glb->Accessors[0].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[1].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[1].Type == GLTF::AccessorType::Vec3);
    POMDOG_ASSERT(glb->Accessors[1].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[2].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[2].Type == GLTF::AccessorType::Vec2);
    POMDOG_ASSERT(glb->Accessors[2].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[3].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[3].Type == GLTF::AccessorType::Scalar);
    POMDOG_ASSERT(glb->Accessors[3].ComponentType == GLTF::ComponentType::Ushort);

    {
        using VertexCombined = BasicEffect::VertexPositionNormalTexture;

        // NOTE: Create vertex buffer
        std::vector<VertexCombined> verticesCombo;

        verticesCombo.resize(glb->Accessors[positionAccessor].Count);

        {
            auto& accessor = glb->Accessors[positionAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].position = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[normalAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].normal = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[texcoordAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector2*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].textureCoordinate = vertexData[i];
            }
        }

        if (auto [vertexBuffer1, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer1_ = std::move(vertexBuffer1);
        }
    }
    {
        using VertexCombined = BasicEffect::VertexPositionColor;

        // NOTE: Create vertex buffer
        std::vector<VertexCombined> verticesCombo;

        verticesCombo.resize(glb->Accessors[positionAccessor].Count);

        {
            auto& accessor = glb->Accessors[positionAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].position = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[normalAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].color = Vector4{vertexData[i], 1.0f};
            }
        }

        if (auto [vertexBuffer2, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer2_ = std::move(vertexBuffer2);
        }
    }
    {
        // NOTE: Create index buffer
        std::vector<std::uint16_t> indices;

        indices.resize(glb->Accessors[indexAccessor].Count);

        {
            auto& accessor = glb->Accessors[indexAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto indexData = reinterpret_cast<const std::uint16_t*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                indices[i] = indexData[i];
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
        static_cast<float>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
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

    auto time = static_cast<float>(gameHost_->getClock()->getTotalGameTime().count());
    auto rotateY = math::TwoPi<float> * rotateSpeed * time;

    const auto mouse = gameHost_->getMouse();
    if (mouse->isButtonDown(MouseButtons::Left)) {
        rotateY = -math::TwoPi<float> * (static_cast<float>(mouse->getPosition().x) / static_cast<float>(presentationParameters.backBufferWidth));
    }

    auto modelMatrix =
        Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f}) *
        Matrix4x4::createScale(1.0f + std::cos(time * 5.0f) * 0.1f) *
        Matrix4x4::createRotationY(rotateY) *
        Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

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
    pass.clearStencil = std::uint8_t(0);
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
    commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);
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
