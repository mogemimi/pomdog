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

void QuickStartGame::Initialize()
{
    // Display message in log console
    Log::Verbose("Hello, QuickStart.");

    // Set window name
    window->SetTitle("QuickStart");

    // Create graphics command list
    commandList = std::get<0>(graphicsDevice->CreateGraphicsCommandList());

    // Load a PNG as texture
    if (auto [res, err] = assets->Load<Texture2D>("pomdog.png"); err != nullptr) {
        // Error handling
        Log::Critical("Game", err->ToString());
        gameHost->Exit();
        return;
    }
    else {
        texture = std::move(res);
    }

    {
        // Create sampler state
        sampler = std::get<0>(graphicsDevice->CreateSamplerState(SamplerDescription::CreatePointClamp()));
    }
    {
        // Create vertex buffer
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

        vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable));
    }
    {
        // Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        indexBuffer = std::get<0>(graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable));
    }
    {
        // Create constant buffer
        constantBuffer = std::get<0>(graphicsDevice->CreateConstantBuffer(
            sizeof(MyShaderConstants),
            BufferUsage::Dynamic));
    }
    {
        // For details, see 'struct VertexCombined' members
        auto inputLayout = InputLayoutHelper{}
            .Float3()
            .Float2()
            .CreateInputLayout();

        auto vertexShader = assets->CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("SimpleEffect_VS.glsl")
            .SetHLSLFromFile("SimpleEffect_VS.hlsl", "SimpleEffectVS")
            .SetMetalFromFile("SimpleEffect.metal", "SimpleEffectVS")
            .Build();

        auto pixelShader = assets->CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("SimpleEffect_PS.glsl")
            .SetHLSLFromFile("SimpleEffect_PS.hlsl", "SimpleEffectPS")
            .SetMetalFromFile("SimpleEffect.metal", "SimpleEffectPS")
            .Build();

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // Create pipeline state
        pipelineState = assets->CreateBuilder<PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetInputLayout(inputLayout)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .SetSamplerBindSlot("DiffuseTexture", 0)
            .Build();
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

        // Initialize shader resources
        auto bounds = window->GetClientBounds();
        updateShaderConstants(bounds.Width, bounds.Height);

        // Connect to window resize event notification
        connect(window->ClientSizeChanged, updateShaderConstants);
    }
    {
        // Create timer
        timer = std::make_unique<Timer>(clock);
        timer->SetInterval(std::chrono::milliseconds(500));

        // Connect to timer event notification
        connect(timer->Elapsed, [this] {
            // String formatting using Pomdog::StringFormat
            auto title = StringHelper::Format(
                "QuickStart %3.0f fps, %s frames",
                std::round(clock->GetFrameRate()),
                std::to_string(clock->GetFrameNumber()).c_str());

            // Set window title
            window->SetTitle(title);
        });
    }
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
