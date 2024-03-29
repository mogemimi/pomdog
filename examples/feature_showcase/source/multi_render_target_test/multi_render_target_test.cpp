#include "multi_render_target_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include <cmath>

namespace feature_showcase {

MultiRenderTargetTest::MultiRenderTargetTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> MultiRenderTargetTest::initialize()
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

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::createNonPremultiplied(),
        std::nullopt,
        gpu::SamplerDescriptor::createPointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

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

        std::tie(vertexBuffer, err) = graphicsDevice->createVertexBuffer(
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
        // For details, see 'struct VertexCombined' members
        auto inputLayout = gpu::InputLayoutHelper{}
            .addFloat3() // NOTE: VertexCombined::Position
            .addFloat3() // NOTE: VertexCombined::Normal
            .addFloat2() // NOTE: VertexCombined::TextureCoord
            .createInputLayout();

        // NOTE: Create vertex shader
        auto [vertexShader, vertexShaderErr] = assets->createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .setGLSLFromFile("Shaders/MultiRTVS.glsl")
            .setHLSLFromFile("Shaders/MultiRT.hlsl", "MultiRTVS")
            .setMetalFromFile("Shaders/MultiRT.metal", "MultiRTVS")
            .build();

        if (vertexShaderErr != nullptr) {
            return errors::wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        // NOTE: Create pixel shader
        auto [pixelShader, pixelShaderErr] = assets->createBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .setGLSLFromFile("Shaders/MultiRTPS.glsl")
            .setHLSLFromFile("Shaders/MultiRT.hlsl", "MultiRTPS")
            .setMetalFromFile("Shaders/MultiRT.metal", "MultiRTPS")
            .build();

        if (pixelShaderErr != nullptr) {
            return errors::wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->createBuilder<gpu::PipelineState>()
            .setRenderTargetViewFormats({
                PixelFormat::R8G8B8A8_UNorm, // NOTE: Albedo
                PixelFormat::R10G10B10A2_UNorm, // NOTE: Normal
                PixelFormat::R32_Float, // NOTE: Depth
                PixelFormat::R8G8B8A8_UNorm, // NOTE: Lighting
            })
            .setDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .setPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .setDepthStencilState(gpu::DepthStencilDescriptor::createDefault())
            .setBlendState(gpu::BlendDescriptor::createOpaque())
            .setRasterizerState(gpu::RasterizerDescriptor::createDefault())
            .setInputLayout(inputLayout)
            .setVertexShader(std::move(vertexShader))
            .setPixelShader(std::move(pixelShader))
            .setConstantBufferBindSlot("ModelConstantBuffer", 0)
            .setConstantBufferBindSlot("WorldConstantBuffer", 1)
            .build();
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    // NOTE: Create render target
    std::tie(renderTargetAlbedo, err) = graphicsDevice->createRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R8G8B8A8_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetNormal, err) = graphicsDevice->createRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R10G10B10A2_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetDepth, err) = graphicsDevice->createRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R32_Float);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetLighting, err) = graphicsDevice->createRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R8G8B8A8_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create depth stencil buffer
    std::tie(depthStencilBuffer, err) = graphicsDevice->createDepthStencilBuffer(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }

    auto window = gameHost->getWindow();

    connect(window->clientSizeChanged, [this](int width, int height) {
        renderTargetAlbedo = std::get<0>(graphicsDevice->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetAlbedo->getFormat()));

        renderTargetNormal = std::get<0>(graphicsDevice->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetNormal->getFormat()));

        renderTargetDepth = std::get<0>(graphicsDevice->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetDepth->getFormat()));

        renderTargetLighting = std::get<0>(graphicsDevice->createRenderTarget2D(
            width,
            height,
            false,
            renderTargetLighting->getFormat()));

        depthStencilBuffer = std::get<0>(graphicsDevice->createDepthStencilBuffer(
            width,
            height,
            depthStencilBuffer->getFormat()));
    });

    return nullptr;
}

void MultiRenderTargetTest::update()
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

void MultiRenderTargetTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    commandList->reset();

    {
        gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
        gpu::RenderPass pass;
        pass.renderTargets[0] = {renderTargetAlbedo, Color::createCornflowerBlue().toVector4()};
        pass.renderTargets[1] = {renderTargetNormal, Vector4{0.0f, 0.0f, 1.0f, 1.0f}};
        pass.renderTargets[2] = {renderTargetDepth, Vector4{0.0f, 0.0f, 0.0f, 1.0f}};
        pass.renderTargets[3] = {renderTargetLighting, Color::createCornflowerBlue().toVector4()};
        pass.depthStencilBuffer = depthStencilBuffer;
        pass.clearDepth = 1.0f;
        pass.clearStencil = std::uint8_t(0);
        pass.viewport = viewport;
        pass.scissorRect = viewport.getBounds();

        commandList->setRenderPass(std::move(pass));
        commandList->setConstantBuffer(0, modelConstantBuffer);
        commandList->setConstantBuffer(1, worldConstantBuffer);
        commandList->setSamplerState(0, sampler);
        commandList->setTexture(0, texture);
        commandList->setVertexBuffer(0, vertexBuffer);
        commandList->setPipelineState(pipelineState);

        commandList->setIndexBuffer(indexBuffer);
        commandList->drawIndexed(indexBuffer->getIndexCount(), 0);
    }
    {
        gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
        gpu::RenderPass pass;
        pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
        pass.depthStencilBuffer = nullptr;
        pass.clearDepth = 1.0f;
        pass.clearStencil = std::uint8_t(0);
        pass.viewport = viewport;
        pass.scissorRect = viewport.getBounds();

        const auto w = static_cast<float>(presentationParameters.backBufferWidth);
        const auto h = static_cast<float>(presentationParameters.backBufferHeight);
        const auto projectionMatrix = Matrix4x4::createOrthographicLH(w, h, 0.0f, 100.0f);

        commandList->setRenderPass(std::move(pass));

        spriteBatch->begin(commandList, projectionMatrix);

        auto draw = [&](std::shared_ptr<gpu::RenderTarget2D> rt, Vector2 pos) {
            auto originPivot = Vector2::createZero();
            auto scale = Vector2{0.5f, 0.5f};
            if (graphicsDevice->getSupportedLanguage() == gpu::ShaderLanguage::GLSL) {
                // NOTE: Flip horizontally for OpenGL coordinate system.
                originPivot.y = 1.0f;
                scale.y = -0.5f;
            }
            spriteBatch->draw(
                rt,
                pos,
                Rectangle{0, 0, rt->getWidth(), rt->getHeight()},
                Color::createWhite(),
                0.0f,
                originPivot,
                scale);
        };

        draw(renderTargetAlbedo, Vector2{-w / 2, 0.0f});
        draw(renderTargetNormal, Vector2{0.0f, 0.0f});
        draw(renderTargetDepth, Vector2{-w / 2, -h / 2});
        draw(renderTargetLighting, Vector2{0.0f, -h / 2});

        spriteBatch->end();
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
