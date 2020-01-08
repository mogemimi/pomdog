#include "BasicEffectTest.hpp"
#include <Pomdog/Experimental/Graphics/BasicEffect.hpp>
#include <cmath>

namespace FeatureShowcase {

BasicEffectTest::BasicEffectTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void BasicEffectTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

    // NOTE: Load texture from image file
    if (auto [res, err] = assets->Load<Texture2D>("Textures/pomdog.png"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        texture = std::move(res);
    }

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

        vertexBuffer1 = std::make_shared<VertexBuffer>(
            graphicsDevice,
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);
    }
    {
        using VertexCombined = BasicEffect::VertexPositionColor;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Color::White.ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::White.ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::White.ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::White.ToVector4()},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Color::Yellow.ToVector4()},
            {Vector3{0.0f, 0.0f, 0.0f}, Color::Yellow.ToVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::Yellow.ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::Yellow.ToVector4()},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Color::Red.ToVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::Red.ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::Red.ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::Red.ToVector4()},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Color::Blue.ToVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::Blue.ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::Blue.ToVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::Blue.ToVector4()},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Color::Red.ToVector4()},
            {Vector3{1.0f, 0.0f, 1.0f}, Color::Yellow.ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::Blue.ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::White.ToVector4()},
        }};

        vertexBuffer2 = std::make_shared<VertexBuffer>(
            graphicsDevice,
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);
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

        indexBuffer = std::make_shared<IndexBuffer>(
            graphicsDevice,
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);
    }
    {
        // NOTE: Create constant buffer
        modelConstantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice,
            sizeof(BasicEffect::ModelConstantBuffer),
            BufferUsage::Dynamic);

        worldConstantBuffer = std::make_shared<ConstantBuffer>(
            graphicsDevice,
            sizeof(BasicEffect::WorldConstantBuffer),
            BufferUsage::Dynamic);
    }
    {
        // NOTE: Create sampler state
        sampler = std::make_shared<SamplerState>(
            graphicsDevice,
            SamplerDescription::CreateLinearClamp());
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = true;
        effectDesc.TextureEnabled = true;
        effectDesc.VertexColorEnabled = false;

        // NOTE: Create pipeline state
        pipelineState1 = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = false;
        effectDesc.TextureEnabled = false;
        effectDesc.VertexColorEnabled = true;

        // NOTE: Create pipeline state
        pipelineState2 = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
    }
}

void BasicEffectTest::Update()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        MathHelper::ToRadians(45.0f),
        static_cast<float>(presentationParameters.BackBufferWidth) / presentationParameters.BackBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 6.0f, 0.0f};
    auto cameraDirection = Vector3{0.0f, -1.0f, 1.0f};
    auto viewMatrix = Matrix4x4::CreateLookAtLH(cameraPosition, cameraPosition + cameraDirection, Vector3::UnitY);

    auto lightDirection = Vector3::Normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.View = viewMatrix;
    worldConstants.Projection = projectionMatrix;
    worldConstants.InverseView = Matrix4x4::Invert(viewMatrix);
    worldConstants.LightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer->SetValue(worldConstants);

    auto time = static_cast<float>(gameHost->GetClock()->GetTotalGameTime().count());
    auto rotateY = Math::TwoPi<float> * rotateSpeed * time;

    auto mouse = gameHost->GetMouse()->GetState();
    if (mouse.LeftButton == ButtonState::Pressed) {
        rotateY = -Math::TwoPi<float> * (static_cast<float>(mouse.Position.X) / static_cast<float>(presentationParameters.BackBufferWidth));
    }

    auto modelMatrix = Matrix4x4::CreateTranslation(Vector3{-0.5f, -0.5f, -0.5f})
        * Matrix4x4::CreateScale(1.0f + std::cos(time * 5.0f) * 0.1f)
        * Matrix4x4::CreateRotationY(rotateY)
        * Matrix4x4::CreateTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.Model = modelMatrix;
    modelConstants.Material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.Color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->SetValue(modelConstants);
}

void BasicEffectTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    auto mouse = gameHost->GetMouse()->GetState();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));
    commandList->SetConstantBuffer(0, modelConstantBuffer);
    commandList->SetConstantBuffer(1, worldConstantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    if (mouse.RightButton == ButtonState::Pressed) {
        commandList->SetVertexBuffer(0, vertexBuffer2);
        commandList->SetPipelineState(pipelineState2);
    }
    else {
        commandList->SetVertexBuffer(0, vertexBuffer1);
        commandList->SetPipelineState(pipelineState1);
    }
    commandList->SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList->DrawIndexed(indexBuffer, indexBuffer->GetIndexCount(), 0);
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
