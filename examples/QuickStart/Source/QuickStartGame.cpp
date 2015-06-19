#include "QuickStartGame.hpp"
#include <utility>
#include <cmath>

namespace QuickStart {
//-----------------------------------------------------------------------
QuickStartGame::QuickStartGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->Window())
    , graphicsDevice(gameHostIn->GraphicsDevice())
    , commandQueue(gameHostIn->GraphicsCommandQueue())
    , assets(gameHostIn->AssetManager())
    , clock(gameHostIn->Clock())
{
}
//-----------------------------------------------------------------------
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

        std::array<VertexCombined, 4> verticesCombo = {
            Vector3(-0.8f, -0.8f, 0.0f), Vector2(0.0f, 1.0f),
            Vector3(-0.8f,  0.8f, 0.0f), Vector2(0.0f, 0.0f),
            Vector3( 0.8f,  0.8f, 0.0f), Vector2(1.0f, 0.0f),
            Vector3( 0.8f, -0.8f, 0.0f), Vector2(1.0f, 1.0f),
        };

        vertexBuffer = std::make_shared<VertexBuffer>(
            graphicsDevice,
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);
    }
    {
        // Create index buffer
        std::array<std::uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};

        indexBuffer = std::make_shared<IndexBuffer>(graphicsDevice,
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);
    }
    {
        // For details, see 'struct VertexCombined' members
        auto inputLayout = InputLayoutHelper{}
            .Float3()
            .Float2();

        auto vertexShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("SimpleEffect_VS.glsl")
            .SetHLSLFromFile("SimpleEffect_VS.hlsl", "SimpleEffectVS");

        auto pixelShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("SimpleEffect_PS.glsl")
            .SetHLSLFromFile("SimpleEffect_PS.hlsl", "SimpleEffectPS");

        auto pipelineBuilder = assets->CreateBuilder<PipelineState>();

        // Create pipeline state
        pipelineState = pipelineBuilder
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .Build();

        // Create constant buffers
        constantBuffers = pipelineBuilder.CreateConstantBuffers(pipelineState);

        // Get constant buffer
        constantBuffer = constantBuffers->FindConstantBuffer("MyConstants");
    }
    {
        // Create graphics command list
        commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

        auto createGraphicsCommands = [this](int width, int height) {
            Viewport viewport = {0, 0, width, height};
            commandList->Reset();
            commandList->SetRenderTarget();
            commandList->SetViewport(viewport);
            commandList->SetScissorRectangle(viewport.GetBounds());
            commandList->Clear(Color::CornflowerBlue);
            commandList->SetSamplerState(0, sampler);
            commandList->SetTexture(0, texture);
            commandList->SetVertexBuffer(vertexBuffer);
            commandList->SetPipelineState(pipelineState);
            commandList->SetConstantBuffers(constantBuffers);
            commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
            commandList->DrawIndexed(indexBuffer, indexBuffer->IndexCount());
            commandList->Close();
        };

        auto bounds = window->GetClientBounds();
        createGraphicsCommands(bounds.Width, bounds.Height);

        // Window resize event
        connect(window->ClientSizeChanged, createGraphicsCommands);
    }
    {
        // Create timer
        timer = std::make_unique<Timer>(clock);
        timer->SetInterval(std::chrono::milliseconds(500));

        // Timer event
        connect(timer->Elapsed, [this] {
            // String formatting using Pomdog::StringFormat
            auto title = StringFormat(
                "QuickStart %3.0f fps, %d frames",
                std::round(clock->FrameRate()),
                clock->FrameNumber());

            // Set window title
            window->SetTitle(title);
        });
    }
}
//-----------------------------------------------------------------------
void QuickStartGame::Update()
{
    auto totalTime = static_cast<float>(clock->TotalGameTime().count());

    struct MyConstants {
        Vector2 Rotation;
    };

    MyConstants myShaderConstants;
    myShaderConstants.Rotation = Vector2{
        std::cos(totalTime) * 0.5f + 0.5f,
        std::sin(totalTime) * 0.5f + 0.5f};

    constantBuffer->SetValue(myShaderConstants);
}
//-----------------------------------------------------------------------
void QuickStartGame::Draw()
{
    commandQueue->Reset();
    commandQueue->PushbackCommandList(commandList);
    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}
//-----------------------------------------------------------------------
} // namespace QuickStart
