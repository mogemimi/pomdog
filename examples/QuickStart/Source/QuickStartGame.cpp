#include "QuickStartGame.hpp"
#include <utility>
#include <cmath>

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

    // Load a PNG as texture
    texture = assets->Load<Texture2D>("pomdog.png");

    // Create sampler state
    sampler = std::make_shared<SamplerState>(
        graphicsDevice,
        SamplerDescription::CreatePointClamp());

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

        vertexBuffer = std::make_shared<VertexBuffer>(
            graphicsDevice,
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);
    }
    {
        // Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        indexBuffer = std::make_shared<IndexBuffer>(
            graphicsDevice,
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);
    }
    {
        // Create constant buffer
        constantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice,
            sizeof(MyShaderConstants),
            BufferUsage::Dynamic);
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
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .Build();
    }
    {
        // Create graphics command list
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        auto updateShaderConstants = [this](int width, int height) {
            auto view = Matrix4x4::Identity;
            auto projection = Matrix4x4::CreateOrthographicLH(width, height, 0, 100);
            myShaderConstants.ViewProjection = Matrix4x4::Transpose(view * projection);
        };

        auto createGraphicsCommands = [this](int width, int height) {
            Viewport viewport = {0, 0, width, height};
            RenderPass pass;
            pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
            pass.ClearDepth = 1.0f;
            pass.ClearStencil = 0;
            pass.Viewport = viewport;
            pass.ScissorRect = viewport.GetBounds();

            commandList->Reset();
            commandList->SetRenderPass(std::move(pass));
            commandList->SetConstantBuffer(0, constantBuffer);
            commandList->SetSamplerState(0, sampler);
            commandList->SetTexture(0, texture);
            commandList->SetVertexBuffer(vertexBuffer);
            commandList->SetPipelineState(pipelineState);
            commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
            commandList->DrawIndexed(indexBuffer, indexBuffer->GetIndexCount(), 0);
            commandList->Close();
        };

        // Initialize shader resources
        auto bounds = window->GetClientBounds();
        updateShaderConstants(bounds.Width, bounds.Height);
        createGraphicsCommands(bounds.Width, bounds.Height);

        // Connect to window resize event notification
        connect(window->ClientSizeChanged, updateShaderConstants);
        connect(window->ClientSizeChanged, createGraphicsCommands);
    }
    {
        // Create timer
        timer = std::make_unique<Timer>(clock);
        timer->SetInterval(std::chrono::milliseconds(500));

        // Timer event
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
    auto scale = Matrix4x4::CreateScale(Vector3(texture->GetWidth(), texture->GetHeight(), 1.0f));
    auto model = scale * rotate;

    myShaderConstants.Model = Matrix4x4::Transpose(model);
    constantBuffer->SetValue(myShaderConstants);
}

void QuickStartGame::Draw()
{
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

} // namespace QuickStart
