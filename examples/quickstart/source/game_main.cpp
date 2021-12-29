#include "game_main.h"
#include <cmath>
#include <utility>

namespace quickstart {

GameMain::GameMain(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->GetWindow())
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , assets(gameHostIn->GetAssetManager())
    , clock(gameHostIn->GetClock())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> GameMain::Initialize()
{
    // NOTE: Display message in log console
    Log::Verbose("Hello, quickstart.");

    // NOTE: Set window name
    window->SetTitle("quickstart");

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load a PNG image as texture
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("pomdog.png");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Create sampler state
    std::tie(sampler, err) = graphicsDevice->CreateSamplerState(gpu::SamplerDescriptor::CreatePointClamp());
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create sampler state");
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

        std::tie(vertexBuffer, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
            gpu::IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            gpu::BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(constantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(MyShaderConstants),
            gpu::BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: For details, see 'struct VertexCombined' members
        auto inputLayout = gpu::InputLayoutHelper{}
            .Float3()
            .Float2()
            .CreateInputLayout();

        auto [vertexShader, vertexShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("simple_effect_vs.glsl")
            .SetHLSLFromFile("simple_effect_vs.hlsl", "SimpleEffectVS")
            .SetMetalFromFile("simple_effect.metal", "SimpleEffectVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            return errors::Wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        auto [pixelShader, pixelShaderErr] = assets->CreateBuilder<gpu::Shader>(gpu::ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("simple_effect_ps.glsl")
            .SetHLSLFromFile("simple_effect_ps.hlsl", "SimpleEffectPS")
            .SetMetalFromFile("simple_effect.metal", "SimpleEffectPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            return errors::Wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->CreateBuilder<gpu::PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetInputLayout(inputLayout)
            .SetPrimitiveTopology(gpu::PrimitiveTopology::TriangleList)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .SetSamplerBindSlot("DiffuseTexture", 0)
            .Build();

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        auto updateShaderConstants = [this]([[maybe_unused]] int width, [[maybe_unused]] int height) {
            const auto presentationParameters = graphicsDevice->GetPresentationParameters();

            const auto viewMatrix = Matrix4x4::Identity();
            const auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
                static_cast<float>(presentationParameters.BackBufferWidth),
                static_cast<float>(presentationParameters.BackBufferHeight),
                0.0f,
                100.0f);
            myShaderConstants.ViewProjection = viewMatrix * projectionMatrix;
        };

        // NOTE: Initialize shader resources
        auto bounds = window->GetClientBounds();
        updateShaderConstants(bounds.Width, bounds.Height);

        // NOTE: Connect to window resize event notification
        connect(window->ClientSizeChanged, updateShaderConstants);
    }
    {
        // NOTE: Create timer
        timer = std::make_unique<Timer>(clock);
        timer->SetInterval(std::chrono::milliseconds(500));

        // NOTE: Connect to timer event notification
        connect(timer->Elapsed, [this] {
            // String formatting using pomdog::StringFormat
            auto title = StringHelper::Format(
                "quickstart %3.0f fps, %s frames",
                std::round(clock->GetFrameRate()),
                std::to_string(clock->GetFrameNumber()).c_str());

            // NOTE: Set window title
            window->SetTitle(title);
        });
    }

    return nullptr;
}

void GameMain::Update()
{
    auto totalTime = static_cast<float>(clock->GetTotalGameTime().count());

    auto rotate = Matrix4x4::CreateRotationZ(std::cos(totalTime));
    auto scale = Matrix4x4::CreateScale(Vector3{
        static_cast<float>(texture->GetWidth()),
        static_cast<float>(texture->GetHeight()),
        1.0f});

    myShaderConstants.Model = scale * rotate;
}

void GameMain::Draw()
{
    const auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    gpu::RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    // Reset graphics command list
    commandList->Reset();

    // Update constant buffer
    constantBuffer->SetValue(myShaderConstants);

    // Create graphics commands
    commandList->SetRenderPass(std::move(pass));
    commandList->SetPipelineState(pipelineState);
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetIndexBuffer(indexBuffer);
    commandList->DrawIndexed(indexBuffer->GetIndexCount(), 0);
    commandList->Close();

    // Submit graphics command list for execution
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

} // namespace quickstart
