#include "game_main.h"
#include <cmath>
#include <utility>

namespace quickstart {

GameMain::GameMain(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->getWindow())
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , assets(gameHostIn->getAssetManager())
    , clock(gameHostIn->getClock())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> GameMain::initialize()
{
    // NOTE: Display message in log console
    Log::Verbose("Hello, quickstart.");

    // NOTE: Set window name
    window->setTitle("quickstart");

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load a PNG image as texture
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Create sampler state
    std::tie(sampler, err) = graphicsDevice->createSamplerState(gpu::SamplerDescriptor::createPointClamp());
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sampler state");
    }

    {
        // NOTE: Create vertex buffer
        struct VertexCombined {
            Vector3 Position;
            Vector2 TextureCoord;
        };

        std::array<VertexCombined, 4> verticesCombo = {{
            VertexCombined{Vector3{-1.0f, -1.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            VertexCombined{Vector3{-1.0f,  1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            VertexCombined{Vector3{ 1.0f,  1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            VertexCombined{Vector3{ 1.0f, -1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
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
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

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
        std::tie(constantBuffer, err) = graphicsDevice->createConstantBuffer(
            sizeof(MyShaderConstants),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: For details, see 'struct VertexCombined' members
        auto inputLayout = gpu::InputLayoutHelper{}
            .addFloat3()
            .addFloat2()
            .createInputLayout();

        auto [vertexShader, vertexShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("simple_effect_vs.glsl")
            .SetHLSLFromFile("simple_effect_vs.hlsl", "SimpleEffectVS")
            .SetMetalFromFile("simple_effect.metal", "SimpleEffectVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            return errors::wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        auto [pixelShader, pixelShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("simple_effect_ps.glsl")
            .SetHLSLFromFile("simple_effect_ps.hlsl", "SimpleEffectPS")
            .SetMetalFromFile("simple_effect.metal", "SimpleEffectPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            return errors::wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->getPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->CreateBuilder<gpu::PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.backBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.depthStencilFormat)
            .SetInputLayout(inputLayout)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .SetSamplerBindSlot("DiffuseTexture", 0)
            .Build();

        if (err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        auto updateShaderConstants = [this]([[maybe_unused]] int width, [[maybe_unused]] int height) {
            const auto presentationParameters = graphicsDevice->getPresentationParameters();

            const auto viewMatrix = Matrix4x4::createIdentity();
            const auto projectionMatrix = Matrix4x4::createOrthographicLH(
                static_cast<float>(presentationParameters.backBufferWidth),
                static_cast<float>(presentationParameters.backBufferHeight),
                0.0f,
                100.0f);
            myShaderConstants.ViewProjection = viewMatrix * projectionMatrix;
        };

        // NOTE: Initialize shader resources
        const auto bounds = window->getClientBounds();
        updateShaderConstants(bounds.width, bounds.height);

        // NOTE: Connect to window resize event notification
        connect(window->clientSizeChanged, updateShaderConstants);
    }
    {
        // NOTE: Create timer
        timer = std::make_unique<Timer>(clock);
        timer->setInterval(std::chrono::milliseconds(500));

        // NOTE: Connect to timer event notification
        connect(timer->elapsed, [this] {
            // String formatting using pomdog::StringFormat
            auto title = strings::format(
                "quickstart %3.0f fps, %s frames",
                std::round(clock->getFrameRate()),
                std::to_string(clock->getFrameNumber()).c_str());

            // NOTE: Set window title
            window->setTitle(title);
        });
    }

    return nullptr;
}

void GameMain::update()
{
    auto totalTime = static_cast<float>(clock->getTotalGameTime().count());

    auto rotate = Matrix4x4::createRotationZ(std::cos(totalTime));
    auto scale = Matrix4x4::createScale(Vector3{
        static_cast<float>(texture->getWidth()),
        static_cast<float>(texture->getHeight()),
        1.0f});

    myShaderConstants.Model = scale * rotate;
}

void GameMain::draw()
{
    const auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    // Reset graphics command list
    commandList->reset();

    // Update constant buffer
    constantBuffer->setData(0, gpu::makeByteSpan(myShaderConstants));

    // Create graphics commands
    commandList->setRenderPass(std::move(pass));
    commandList->setPipelineState(pipelineState);
    commandList->setConstantBuffer(0, constantBuffer);
    commandList->setSamplerState(0, sampler);
    commandList->setTexture(0, texture);
    commandList->setVertexBuffer(0, vertexBuffer);
    commandList->setIndexBuffer(indexBuffer);
    commandList->drawIndexed(indexBuffer->getIndexCount(), 0);
    commandList->close();

    // Submit graphics command list for execution
    commandQueue->reset();
    commandQueue->pushBackCommandList(commandList);
    commandQueue->executeCommandLists();
    commandQueue->present();
}

} // namespace quickstart
