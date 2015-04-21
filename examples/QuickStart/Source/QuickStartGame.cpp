#include "QuickStartGame.hpp"
#include <utility>
#include <cmath>

namespace QuickStart {
//-----------------------------------------------------------------------
QuickStartGame::QuickStartGame(std::shared_ptr<GameHost> const& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsContext(gameHostIn->GraphicsContext())
{
}
//-----------------------------------------------------------------------
void QuickStartGame::Initialize()
{
    auto window = gameHost->Window();
    window->Title("QuickStart");
    window->AllowPlayerResizing(false);

    auto graphicsDevice = gameHost->GraphicsDevice();
    auto assets = gameHost->AssetManager();

    {
        struct VertexCombined {
            Vector3 Position;
            Vector2 TextureCoord;
        };

        std::array<VertexCombined, 4> const verticesCombo = {
            Vector3(-0.8f, -0.8f, 0.0f), Vector2(0.0f, 1.0f),
            Vector3(-0.8f,  0.8f, 0.0f), Vector2(0.0f, 0.0f),
            Vector3( 0.8f,  0.8f, 0.0f), Vector2(1.0f, 0.0f),
            Vector3( 0.8f, -0.8f, 0.0f), Vector2(1.0f, 1.0f),
        };

        // Create vertex buffer
        vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
            verticesCombo.data(), verticesCombo.size(),
            sizeof(VertexCombined), BufferUsage::Immutable);
    }
    {
        std::array<std::uint16_t, 6> const indices = {
            0, 1, 2,
            2, 3, 0
        };

        // Create index buffer
        indexBuffer = std::make_shared<IndexBuffer>(graphicsDevice,
            IndexElementSize::SixteenBits,
            indices.data(), indices.size(), BufferUsage::Immutable);
    }
    {
        auto inputLayout = InputLayoutHelper{}
            .Float3().Float2();

        auto vertexShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("SimpleEffect_VS.glsl")
            .SetHLSLFromFile("SimpleEffect_VS.hlsl", "SimpleEffectVS");

        auto pixelShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("SimpleEffect_PS.glsl")
            .SetHLSLFromFile("SimpleEffect_PS.hlsl", "SimpleEffectPS");

        auto builder = assets->CreateBuilder<PipelineState>();
        pipelineState = builder
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .Build();

        constantBuffers = builder.CreateConstantBuffers(pipelineState);

        constantBuffer = constantBuffers->Find("MyConstants");
    }
    {
        sampler = std::make_shared<SamplerState>(graphicsDevice,
            SamplerDescription::CreatePointClamp());

        // Load a PNG as texture
        texture = assets->Load<Texture2D>("pomdog.png");
    }
    {
        renderTarget = std::make_shared<RenderTarget2D>(graphicsDevice,
            window->ClientBounds().Width, window->ClientBounds().Height);
    }
}
//-----------------------------------------------------------------------
void QuickStartGame::Update()
{
    auto clock = gameHost->Clock();
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
    graphicsContext->Clear(Color::CornflowerBlue);

    graphicsContext->SetSamplerState(0, sampler);
    graphicsContext->SetTexture(0, texture);
    graphicsContext->SetVertexBuffer(vertexBuffer);
    graphicsContext->SetPipelineState(pipelineState);
    graphicsContext->SetConstantBuffers(constantBuffers);
    graphicsContext->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    graphicsContext->DrawIndexed(indexBuffer, indexBuffer->IndexCount());

    graphicsContext->Present();
}
//-----------------------------------------------------------------------
} // namespace QuickStart
