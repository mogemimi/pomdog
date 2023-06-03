#include "gltf_model_test.h"
#include "pomdog/experimental/gltf/gltf.h"
#include "pomdog/experimental/gltf/gltf_loader.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include <cmath>

namespace feature_showcase {

GLTFModelTest::GLTFModelTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> GLTFModelTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load texture from image file
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load model from glTF binary file.
    auto [glb, glbErr] = GLTF::Open(assets->GetAssetPath("GLB/F15.glb"));
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

        std::tie(vertexBuffer1, err) = graphicsDevice->createVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
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

        std::tie(vertexBuffer2, err) = graphicsDevice->createVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
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
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->createSamplerState(
            gpu::SamplerDescriptor::createLinearClamp());

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
    }
    {
        auto presentationParameters = graphicsDevice->getPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.lightingEnabled = true;
        effectDesc.textureEnabled = true;
        effectDesc.vertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState1, err) = BasicEffect::createBasicEffect(*assets, effectDesc)
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
    }
    {
        auto presentationParameters = graphicsDevice->getPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.lightingEnabled = false;
        effectDesc.textureEnabled = false;
        effectDesc.vertexColorEnabled = true;

        // NOTE: Create pipeline state
        std::tie(pipelineState2, err) = BasicEffect::createBasicEffect(*assets, effectDesc)
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
    }

    return nullptr;
}

void GLTFModelTest::update()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadians(45.0f),
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
    worldConstantBuffer->setData(0, gpu::makeByteSpan(worldConstants));

    auto time = static_cast<float>(gameHost->getClock()->getTotalGameTime().count());
    auto rotateY = math::TwoPi<float> * rotateSpeed * time;

    const auto mouse = gameHost->getMouse()->getState();
    if (mouse.leftButton == ButtonState::Pressed) {
        rotateY = -math::TwoPi<float> * (static_cast<float>(mouse.position.x) / static_cast<float>(presentationParameters.backBufferWidth));
    }

    auto modelMatrix = Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f})
        * Matrix4x4::createScale(1.0f + std::cos(time * 5.0f) * 0.1f)
        * Matrix4x4::createRotationY(rotateY)
        * Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.model = modelMatrix;
    modelConstants.material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->setData(0, gpu::makeByteSpan(modelConstants));
}

void GLTFModelTest::draw()
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

    const auto mouse = gameHost->getMouse()->getState();

    commandList->reset();
    commandList->setRenderPass(std::move(pass));
    commandList->setConstantBuffer(0, modelConstantBuffer);
    commandList->setConstantBuffer(1, worldConstantBuffer);
    commandList->setSamplerState(0, sampler);
    commandList->setTexture(0, texture);
    if (mouse.rightButton == ButtonState::Pressed) {
        commandList->setVertexBuffer(0, vertexBuffer2);
        commandList->setPipelineState(pipelineState2);
    }
    else {
        commandList->setVertexBuffer(0, vertexBuffer1);
        commandList->setPipelineState(pipelineState1);
    }
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexed(indexBuffer->getIndexCount(), 0);
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
