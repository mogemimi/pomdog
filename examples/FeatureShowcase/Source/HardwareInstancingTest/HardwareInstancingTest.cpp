#include "HardwareInstancingTest.hpp"
#include <cmath>
#include <random>

namespace FeatureShowcase {

HardwareInstancingTest::HardwareInstancingTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void HardwareInstancingTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::get<0>(graphicsDevice->CreateGraphicsCommandList());

    // NOTE: Load texture from image file
    if (auto [res, err] = assets->Load<Texture2D>("Textures/pomdog.png"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texture = std::move(res);
    }

    constexpr auto maxSpriteCount = 256;

    {
        // NOTE: Create vertex buffer
        struct VertexCombined final {
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
        // NOTE: Create index buffer
        std::array<std::uint16_t, 6> indices = {{0, 1, 2, 2, 3, 0}};

        indexBuffer = std::get<0>(graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable));
    }
    {
        instanceBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
            maxSpriteCount,
            sizeof(SpriteInfo),
            BufferUsage::Dynamic));
    }
    {
        // NOTE: Create constant buffer
        constantBuffer = std::get<0>(graphicsDevice->CreateConstantBuffer(
            sizeof(Matrix4x4),
            BufferUsage::Dynamic));
    }
    {
        // NOTE: Create sampler state
        sampler = std::get<0>(graphicsDevice->CreateSamplerState(
            SamplerDescription::CreateLinearClamp()));
    }
    {
        // For details, see 'struct VertexCombined' members
        auto inputLayout = InputLayoutHelper{}
            .Float3() // NOTE: VertexCombined::Position
            .Float2() // NOTE: VertexCombined::TextureCoord
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float4() // NOTE: SpriteInfo::Translation
            .Float4() // NOTE: SpriteInfo::Color
            .CreateInputLayout();

        auto [vertexShader, vertexShaderErr] = assets->CreateBuilder<Shader>(ShaderPipelineStage::VertexShader)
            .SetGLSLFromFile("Shaders/HardwareInstancingVS.glsl")
            .SetHLSLFromFile("Shaders/HardwareInstancing.hlsl", "HardwareInstancingVS")
            .SetMetalFromFile("Shaders/HardwareInstancing.metal", "HardwareInstancingVS")
            .Build();

        if (vertexShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto [pixelShader, pixelShaderErr] = assets->CreateBuilder<Shader>(ShaderPipelineStage::PixelShader)
            .SetGLSLFromFile("Shaders/HardwareInstancingPS.glsl")
            .SetHLSLFromFile("Shaders/HardwareInstancing.hlsl", "HardwareInstancingPS")
            .SetMetalFromFile("Shaders/HardwareInstancing.metal", "HardwareInstancingPS")
            .Build();

        if (pixelShaderErr != nullptr) {
            // FIXME: error handling
        }

        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        // NOTE: Create pipeline state
        std::shared_ptr<Error> err;
        std::tie(pipelineState, err) = assets->CreateBuilder<PipelineState>()
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetInputLayout(inputLayout)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetVertexShader(std::move(vertexShader))
            .SetPixelShader(std::move(pixelShader))
            .SetConstantBufferBindSlot("MyShaderConstants", 0)
            .Build();

        if (err != nullptr) {
            // FIXME: error handling
        }
    }
    {
        SpriteInfo sprite;
        sprite.Translation = Vector4{0.0f, 0.0f, 32.0f, 32.0f};
        sprite.Color = Color::White.ToVector4();

        // NOTE: Add new sprite
        sprites.push_back(std::move(sprite));
    }

    auto mouse = gameHost->GetMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        if (sprites.size() >= instanceBuffer->GetVertexCount()) {
            return;
        }

        auto window = gameHost->GetWindow();
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        auto pos = mouseState.Position;
        pos.X = pos.X - (window->GetClientBounds().Width / 2);
        pos.Y = -pos.Y + (window->GetClientBounds().Height / 2);

        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> scaleDist(16.0f, 32.0f);
        std::uniform_real_distribution<float> colorDist{0.7f, 1.0f};

        const auto scale = scaleDist(random);

        SpriteInfo sprite;
        sprite.Translation.X = static_cast<float>(pos.X);
        sprite.Translation.Y = static_cast<float>(pos.Y);
        sprite.Translation.Z = scale;
        sprite.Translation.W = scale;
        sprite.Color.X = colorDist(random);
        sprite.Color.Y = colorDist(random);
        sprite.Color.Z = colorDist(random);
        sprite.Color.W = 1.0f;

        // NOTE: Add new sprite
        sprites.push_back(std::move(sprite));
    });
}

void HardwareInstancingTest::Update()
{
}

void HardwareInstancingTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto viewMatrix = Matrix4x4::Identity;
    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);
    auto viewProjection = viewMatrix * projectionMatrix;

    // NOTE: Update constant buffer
    constantBuffer->SetValue(viewProjection);

    // NOTE: Update instance buffer
    instanceBuffer->SetData(sprites.data(), sprites.size());

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));
    commandList->SetConstantBuffer(0, constantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    commandList->SetVertexBuffer(0, vertexBuffer);
    commandList->SetVertexBuffer(1, instanceBuffer);
    commandList->SetPipelineState(pipelineState);
    commandList->DrawIndexedInstanced(indexBuffer, indexBuffer->GetIndexCount(), sprites.size(), 0, 0);
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

} // namespace FeatureShowcase
