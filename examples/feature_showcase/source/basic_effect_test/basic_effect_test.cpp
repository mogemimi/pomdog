#include "basic_effect_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include <cmath>

namespace feature_showcase {

BasicEffectTest::BasicEffectTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> BasicEffectTest::initialize()
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
    std::tie(texture, err) = assets->load<gpu::Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    {
        using VertexCombined = BasicEffect::VertexPositionNormalTexture;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{0.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            {Vector3{0.0f, 0.0f, 0.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{-1.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{1.0f, 0.0f, 0.0f}, Vector2{0.0f, 1.0f}},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f}, Vector2{0.0f, 1.0f}},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{0.0f, 0.0f}},
            {Vector3{1.0f, 0.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{1.0f, 0.0f}},
            {Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{1.0f, 1.0f}},
            {Vector3{0.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 1.0f}, Vector2{0.0f, 1.0f}},
        }};

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
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createWhite().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createWhite().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 0.0f, 0.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createYellow().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createYellow().toVector4()},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createRed().toVector4()},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::createBlue().toVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::createBlue().toVector4()},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Color::createRed().toVector4()},
            {Vector3{1.0f, 0.0f, 1.0f}, Color::createYellow().toVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::createBlue().toVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::createWhite().toVector4()},
        }};

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
        std::array<std::uint16_t, 30> indices = {{
            3, 1, 0,
            2, 1, 3,

            7, 5, 4,
            6, 5, 7,

            10, 8, 9,
            11, 8, 10,

            15, 13, 12,
            14, 13, 15,

            18, 16, 17,
            19, 16, 18,
        }};

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
    }
    {
        auto presentationParameters = graphicsDevice->getPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.lightingEnabled = false;
        effectDesc.textureEnabled = false;
        effectDesc.vertexColorEnabled = true;

        // NOTE: Create pipeline state
        std::tie(pipelineState2, err) = BasicEffect::createBasicEffect(*assets, effectDesc)
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
    }

    return nullptr;
}

void BasicEffectTest::update()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadians(45.0f),
        static_cast<float>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 6.0f, 0.0f};
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

    auto mouse = gameHost->getMouse()->getState();
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

void BasicEffectTest::draw()
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

    auto mouse = gameHost->getMouse()->getState();

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
