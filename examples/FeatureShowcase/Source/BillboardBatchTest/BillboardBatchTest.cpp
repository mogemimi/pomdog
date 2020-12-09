#include "BillboardBatchTest.hpp"
#include <Pomdog/Experimental/Graphics/BasicEffect.hpp>
#include <random>

namespace FeatureShowcase {

BillboardBatchTest::BillboardBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::shared_ptr<Error> BillboardBatchTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::shared_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    lineBatch = std::make_shared<LineBatch>(graphicsDevice, *assets);

    // NOTE: Create billboard batch effect
    billboardEffect = std::make_shared<BillboardBatchEffect>(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        *assets);

    // NOTE: Create billboard batch buffer
    billboardBuffer = std::make_shared<BillboardBatchBuffer>(graphicsDevice, 256);

    // NOTE: Create sampler state
    std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
        SamplerDescription::CreateLinearClamp());
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create sampler state");
    }

    // NOTE: Create constant buffer
    std::tie(constantBuffer, err) = graphicsDevice->CreateConstantBuffer(
        sizeof(BasicEffect::WorldConstantBuffer),
        BufferUsage::Dynamic);
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create constant buffer");
    }

    // NOTE: Load texture from PNG image file.
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.1);

    return nullptr;
}

void BillboardBatchTest::Update()
{
}

void BillboardBatchTest::Draw()
{
    const auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        Math::ToRadians(45.0f),
        static_cast<float>(presentationParameters.BackBufferWidth) / presentationParameters.BackBufferHeight,
        0.01f,
        500.0f);

    const auto totalTime = static_cast<float>(timer->GetTotalTime().count());
    const auto lookAtPosition = Vector3{0.0f, 0.0f, 5.0f};
    const auto rotation = Matrix4x4::CreateRotationY(Math::TwoPi<float> * totalTime);
    const auto cameraPosition = lookAtPosition + Vector3::Transform(Vector3{0.0f, 6.0f, -8.0f}, rotation);
    const auto viewMatrix = Matrix4x4::CreateLookAtLH(cameraPosition, lookAtPosition, Vector3::UnitY);
    const auto viewProjection = viewMatrix * projectionMatrix;

    const auto lightDirection = Vector3::Normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer constants;
    constants.ViewProjection = viewProjection;
    constants.View = viewMatrix;
    constants.Projection = projectionMatrix;
    constants.InverseView = Matrix4x4::Invert(viewMatrix);
    constants.LightDirection = Vector4{lightDirection, 0.0f};
    constantBuffer->SetValue(constants);

    // Drawing line
    lineBatch->Begin(commandList, viewProjection);
    {
        // NOTE: Draw grid
        constexpr int lineCount = 40;
        constexpr float gridSize = 1.0f;
        constexpr float lineLength = gridSize * (lineCount - 1);
        for (int i = 0; i < lineCount; i++) {
            constexpr float startOffsetX = -10.0f;
            constexpr float startOffsetZ = -10.0f;
            float x = static_cast<float>(i) * gridSize + startOffsetX;
            float z = static_cast<float>(i) * gridSize + startOffsetZ;

            auto color = Color::White;
            if (i % 5 != 0) {
                color = Color{255, 255, 255, 100};
            }

            lineBatch->DrawLine(Vector3{x, 0.0f, startOffsetZ}, Vector3{x, 0.0f, lineLength + startOffsetZ}, color);
            lineBatch->DrawLine(Vector3{startOffsetX, 0.0f, z}, Vector3{lineLength + startOffsetX, 0.0f, z}, color);
        }
    }
    lineBatch->End();

    billboardBuffer->Reset();

    billboardBuffer->AddBillboard(
        Vector3{0.0f, 0.0f, 2.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        Color::White, 0.0f,
        Vector2{0.5f, 0.5f},
        Vector2{1.0f, 1.0f});

    billboardBuffer->AddBillboard(
        Vector3{0.0f, 0.0f, 5.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 1.0f},
        Color::Red,
        Math::ToRadians(45.0f),
        Vector2{0.5f, 0.5f},
        Vector2{1.0f, 1.0f});

    billboardBuffer->AddBillboard(
        Vector3{0.0f, 0.0f, 7.0f},
        Vector2{0.0f, 0.0f},
        Vector2{0.5f, 1.0f},
        Color::Green,
        Math::ToRadians(0.0f),
        Vector2{0.5f, 0.0f},
        Vector2{1.0f, 2.0f});

    billboardBuffer->AddBillboard(
        Vector3{0.0f, 0.0f, 10.0f},
        Vector2{0.0f, 0.0f},
        Vector2{1.0f, 0.5f},
        Color::Blue,
        Math::ToRadians(0.0f),
        Vector2{0.0f, 0.0f},
        Vector2{2.0f, 1.0f});

    billboardBuffer->FetchBuffer();
    billboardEffect->Draw(commandList, texture, sampler, constantBuffer, 0, *billboardBuffer);

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
