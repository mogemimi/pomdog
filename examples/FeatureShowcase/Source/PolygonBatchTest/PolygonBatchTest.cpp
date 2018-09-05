#include "PolygonBatchTest.hpp"
#include <Pomdog/Experimental/Tween/EasingHelper.hpp>

namespace FeatureShowcase {

PolygonBatchTest::PolygonBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void PolygonBatchTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    polygonBatch = std::make_shared<PolygonBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
}

void PolygonBatchTest::Update()
{
}

void PolygonBatchTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        0.0f,
        100.0f);

    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    const auto t = static_cast<float>(timer->GetTotalTime().count());

    polygonBatch->Begin(commandList, projectionMatrix);

    // Drawing line
    polygonBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    polygonBatch->DrawRectangle(Vector2::Zero, 100, 40, Vector2{1.0f, 1.0f}, Color::White);
    polygonBatch->DrawRectangle(Vector2::Zero, 40, 100, Vector2{0.0f, 0.0f}, Color::Black);
    polygonBatch->DrawRectangle(Vector2::Zero, 30, 30, Vector2{0.5f, 0.5f}, Color::Green);

    // Drawing arc
    polygonBatch->DrawArc(
        Vector2{0.0f, 100.0f}, 40.0f,
        MathHelper::ToRadians(0.0f), MathHelper::ToRadians(270.0f * Easings::EaseSine::InOut(t)),
        80, Color::Yellow);

    // Drawing triangle
    polygonBatch->DrawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::Black, Color::Green, Color::Red);

    polygonBatch->End();

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
