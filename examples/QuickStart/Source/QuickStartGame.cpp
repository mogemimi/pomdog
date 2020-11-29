#include "QuickStartGame.hpp"
#include <cmath>
#include <utility>

namespace QuickStart {

QuickStartGame::QuickStartGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->GetWindow())
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , assets(gameHostIn->GetAssetManager())
    , clock(gameHostIn->GetClock())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::shared_ptr<Error> QuickStartGame::Initialize()
{
    // NOTE: Display message in log console
    Log::Verbose("Hello, QuickStart.");

    // NOTE: Set window name
    window->SetTitle("QuickStart");

    std::shared_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load a PNG image as texture
    std::tie(texture, err) = assets->Load<Texture2D>("pomdog.png");
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Create sampler state
    std::tie(sampler, err) = graphicsDevice->CreateSamplerState(SamplerDescription::CreatePointClamp());
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create sampler state");
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
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(constantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(MyShaderConstants),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: For details, see 'struct VertexCombined' members
        auto inputLayout = InputLayoutHelper{}
            .Float3()
            .Float2()
            .CreateInputLayout();

        auto [vertexShader, vertexShaderErr] = assets->CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("SimpleEffect_VS.glsl")
            .SetHLSLFromFile("SimpleEffect_VS.hlsl", "SimpleEffectVS")
            .SetMetalFromFile("SimpleEffect.metal", "SimpleEffectVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            return Errors::Wrap(std::move(vertexShaderErr), "failed to create vertex shader");
        }

        auto [pixelShader, pixelShaderErr] = assets->CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("SimpleEffect_PS.glsl")
            .SetHLSLFromFile("SimpleEffect_PS.hlsl", "SimpleEffectPS")
            .SetMetalFromFile("SimpleEffect.metal", "SimpleEffectPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            return Errors::Wrap(std::move(pixelShaderErr), "failed to create pixel shader");
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // NOTE: Create pipeline state
        std::tie(pipelineState, err) = assets->CreateBuilder<PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetInputLayout(inputLayout)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .SetSamplerBindSlot("DiffuseTexture", 0)
            .Build();

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        auto updateShaderConstants = [this]([[maybe_unused]] int width, [[maybe_unused]] int height) {
            const auto presentationParameters = graphicsDevice->GetPresentationParameters();

            const auto viewMatrix = Matrix4x4::Identity;
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
            // String formatting using Pomdog::StringFormat
            auto title = StringHelper::Format(
                "QuickStart %3.0f fps, %s frames",
                std::round(clock->GetFrameRate()),
                std::to_string(clock->GetFrameNumber()).c_str());

            // NOTE: Set window title
            window->SetTitle(title);
        });
    }

    return nullptr;
}

void QuickStartGame::Update()
{
    auto totalTime = static_cast<float>(clock->GetTotalGameTime().count());

    auto rotate = Matrix4x4::CreateRotationZ(std::cos(totalTime));
    auto scale = Matrix4x4::CreateScale(Vector3(static_cast<float>(texture->GetWidth()), static_cast<float>(texture->GetHeight()), 1.0f));

    myShaderConstants.Model = scale * rotate;
}

void QuickStartGame::Draw()
{
    const auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    // Reset graphics command list
    commandList->Reset();

    // Update constant buffer
    constantBuffer->SetValue(myShaderConstants);

    // Create graphics commands
    commandList->SetRenderPass(std::move(pass));
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->DrawIndexed(indexBuffer, indexBuffer->GetIndexCount(), 0);
    commandList->Close();

    // Submit graphics command list for execution
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

} // namespace QuickStart
