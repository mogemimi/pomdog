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
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load texture from image file
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::CreateNonPremultiplied(),
        std::nullopt,
        gpu::SamplerDescriptor::CreatePointWrap(),
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

        std::tie(vertexBuffer, err) = graphicsDevice->CreateVertexBuffer(
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

        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
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
        std::tie(modelConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
            gpu::SamplerDescriptor::CreateLinearClamp());

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sampler state");
        }
    }
    {
        // For details, see 'struct VertexCombined' members
        auto inputLayout = gpu::InputLayoutHelper{}
            .Float3() // NOTE: VertexCombined::Position
            .Float3() // NOTE: VertexCombined::Normal
            .Float2() // NOTE: VertexCombined::TextureCoord
            .CreateInputLayout();

        // NOTE: Create vertex shader
        auto [vertexShader, vertexShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("Shaders/MultiRTVS.glsl")
            .SetHLSLFromFile("Shaders/MultiRT.hlsl", "MultiRTVS")
            .SetMetalFromFile("Shaders/MultiRT.metal", "MultiRTVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            return errors::wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        // NOTE: Create pixel shader
        auto [pixelShader, pixelShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("Shaders/MultiRTPS.glsl")
            .SetHLSLFromFile("Shaders/MultiRT.hlsl", "MultiRTPS")
            .SetMetalFromFile("Shaders/MultiRT.metal", "MultiRTPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            return errors::wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->CreateBuilder<gpu::PipelineState>()
            .SetRenderTargetViewFormats({
                PixelFormat::R8G8B8A8_UNorm, // NOTE: Albedo
                PixelFormat::R10G10B10A2_UNorm, // NOTE: Normal
                PixelFormat::R32_Float, // NOTE: Depth
                PixelFormat::R8G8B8A8_UNorm, // NOTE: Lighting
            })
            .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetDepthStencilState(gpu::DepthStencilDescriptor::CreateDefault())
            .SetBlendState(gpu::BlendDescriptor::CreateOpaque())
            .SetRasterizerState(gpu::RasterizerDescriptor::CreateDefault())
            .SetInputLayout(inputLayout)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("ModelConstantBuffer", 0)
            .SetConstantBufferBindSlot("WorldConstantBuffer", 1)
            .Build();
        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }

    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    // NOTE: Create render target
    std::tie(renderTargetAlbedo, err) = graphicsDevice->CreateRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R8G8B8A8_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetNormal, err) = graphicsDevice->CreateRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R10G10B10A2_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetDepth, err) = graphicsDevice->CreateRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R32_Float);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create render target
    std::tie(renderTargetLighting, err) = graphicsDevice->CreateRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        PixelFormat::R8G8B8A8_UNorm);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create depth stencil buffer
    std::tie(depthStencilBuffer, err) = graphicsDevice->CreateDepthStencilBuffer(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }

    auto window = gameHost->getWindow();

    connect(window->clientSizeChanged, [this](int width, int height) {
        renderTargetAlbedo = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            renderTargetAlbedo->GetFormat()));

        renderTargetNormal = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            renderTargetNormal->GetFormat()));

        renderTargetDepth = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            renderTargetDepth->GetFormat()));

        renderTargetLighting = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            renderTargetLighting->GetFormat()));

        depthStencilBuffer = std::get<0>(graphicsDevice->CreateDepthStencilBuffer(
            width,
            height,
            depthStencilBuffer->GetFormat()));
    });

    return nullptr;
}

void MultiRenderTargetTest::update()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

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
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.View = viewMatrix;
    worldConstants.Projection = projectionMatrix;
    worldConstants.InverseView = math::invert(viewMatrix);
    worldConstants.LightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer->SetData(0, gpu::MakeByteSpan(worldConstants));

    auto time = static_cast<float>(gameHost->getClock()->GetTotalGameTime().count());
    auto rotateY = math::TwoPi<float> * rotateSpeed * time;

    auto mouse = gameHost->getMouse()->GetState();
    if (mouse.LeftButton == ButtonState::Pressed) {
        rotateY = -math::TwoPi<float> * (static_cast<float>(mouse.Position.x) / static_cast<float>(presentationParameters.backBufferWidth));
    }

    auto modelMatrix = Matrix4x4::createTranslation(Vector3{-0.5f, -0.5f, -0.5f})
        * Matrix4x4::createScale(1.0f + std::cos(time * 5.0f) * 0.1f)
        * Matrix4x4::createRotationY(rotateY)
        * Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.Model = modelMatrix;
    modelConstants.Material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.Color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->SetData(0, gpu::MakeByteSpan(modelConstants));
}

void MultiRenderTargetTest::draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    commandList->Reset();

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

        commandList->SetRenderPass(std::move(pass));
        commandList->SetConstantBuffer(0, modelConstantBuffer);
        commandList->SetConstantBuffer(1, worldConstantBuffer);
        commandList->SetSamplerState(0, sampler);
        commandList->SetTexture(0, texture);
        commandList->SetVertexBuffer(0, vertexBuffer);
        commandList->SetPipelineState(pipelineState);

        commandList->SetIndexBuffer(indexBuffer);
        commandList->DrawIndexed(indexBuffer->GetIndexCount(), 0);
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

        commandList->SetRenderPass(std::move(pass));

        spriteBatch->Begin(commandList, projectionMatrix);

        auto draw = [&](std::shared_ptr<gpu::RenderTarget2D> rt, Vector2 pos) {
            auto originPivot = Vector2::createZero();
            auto scale = Vector2{0.5f, 0.5f};
            if (graphicsDevice->GetSupportedLanguage() == gpu::ShaderLanguage::GLSL) {
                // NOTE: Flip horizontally for OpenGL coordinate system.
                originPivot.y = 1.0f;
                scale.y = -0.5f;
            }
            spriteBatch->Draw(
                rt,
                pos,
                Rectangle{0, 0, rt->GetWidth(), rt->GetHeight()},
                Color::createWhite(),
                0.0f,
                originPivot,
                scale);
        };

        draw(renderTargetAlbedo, Vector2{-w / 2, 0.0f});
        draw(renderTargetNormal, Vector2{0.0f, 0.0f});
        draw(renderTargetDepth, Vector2{-w / 2, -h / 2});
        draw(renderTargetLighting, Vector2{0.0f, -h / 2});

        spriteBatch->End();
    }

    commandList->Close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->Reset();
        commandQueue->PushbackCommandList(commandList);
        commandQueue->ExecuteCommandLists();
        commandQueue->Present();
    }
    else {
        commandQueue->PushbackCommandList(commandList);
    }
}

} // namespace feature_showcase
