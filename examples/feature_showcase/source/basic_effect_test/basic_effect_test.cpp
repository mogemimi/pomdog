#include "basic_effect_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include <cmath>

namespace feature_showcase {

BasicEffectTest::BasicEffectTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> BasicEffectTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load texture from image file
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
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

        std::tie(vertexBuffer1, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        using VertexCombined = BasicEffect::VertexPositionColor;

        // NOTE: Create vertex buffer
        std::array<VertexCombined, 20> verticesCombo = {{
            // top
            {Vector3{0.0f, 1.0f, 0.0f}, Color::White().ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::White().ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::White().ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::White().ToVector4()},

            // left
            {Vector3{0.0f, 0.0f, 1.0f}, Color::Yellow().ToVector4()},
            {Vector3{0.0f, 0.0f, 0.0f}, Color::Yellow().ToVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::Yellow().ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::Yellow().ToVector4()},

            // right
            {Vector3{1.0f, 0.0f, 1.0f}, Color::Red().ToVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::Red().ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::Red().ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::Red().ToVector4()},

            // front
            {Vector3{0.0f, 0.0f, 0.0f}, Color::Blue().ToVector4()},
            {Vector3{1.0f, 0.0f, 0.0f}, Color::Blue().ToVector4()},
            {Vector3{1.0f, 1.0f, 0.0f}, Color::Blue().ToVector4()},
            {Vector3{0.0f, 1.0f, 0.0f}, Color::Blue().ToVector4()},

            // back
            {Vector3{0.0f, 0.0f, 1.0f}, Color::Red().ToVector4()},
            {Vector3{1.0f, 0.0f, 1.0f}, Color::Yellow().ToVector4()},
            {Vector3{1.0f, 1.0f, 1.0f}, Color::Blue().ToVector4()},
            {Vector3{0.0f, 1.0f, 1.0f}, Color::White().ToVector4()},
        }};

        std::tie(vertexBuffer2, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create vertex buffer");
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
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(modelConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
            SamplerDescription::CreateLinearClamp());

        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create sampler state");
        }
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = true;
        effectDesc.TextureEnabled = true;
        effectDesc.VertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState1, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = false;
        effectDesc.TextureEnabled = false;
        effectDesc.VertexColorEnabled = true;

        // NOTE: Create pipeline state
        std::tie(pipelineState2, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
        if (err != nullptr) {
            return errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }

    return nullptr;
}

void BasicEffectTest::Update()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        math::ToRadians(45.0f),
        static_cast<float>(presentationParameters.BackBufferWidth) / presentationParameters.BackBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 6.0f, 0.0f};
    auto cameraDirection = Vector3{0.0f, -1.0f, 1.0f};
    auto viewMatrix = Matrix4x4::CreateLookAtLH(cameraPosition, cameraPosition + cameraDirection, Vector3::UnitY());

    auto lightDirection = math::Normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.View = viewMatrix;
    worldConstants.Projection = projectionMatrix;
    worldConstants.InverseView = math::Invert(viewMatrix);
    worldConstants.LightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer->SetValue(worldConstants);

    auto time = static_cast<float>(gameHost->GetClock()->GetTotalGameTime().count());
    auto rotateY = math::TwoPi<float> * rotateSpeed * time;

    auto mouse = gameHost->GetMouse()->GetState();
    if (mouse.LeftButton == ButtonState::Pressed) {
        rotateY = -math::TwoPi<float> * (static_cast<float>(mouse.Position.X) / static_cast<float>(presentationParameters.BackBufferWidth));
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
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
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
    commandList->SetIndexBuffer(indexBuffer);
    commandList->DrawIndexed(indexBuffer->GetIndexCount(), 0);
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
