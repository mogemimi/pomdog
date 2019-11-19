#include "ImageEffectsTest.hpp"
#include <Pomdog/Experimental/ImageEffects/ChromaticAberration.hpp>
#include <Pomdog/Experimental/ImageEffects/FXAA.hpp>
#include <Pomdog/Experimental/ImageEffects/FishEyeEffect.hpp>
#include <Pomdog/Experimental/ImageEffects/RetroCrtEffect.hpp>
#include <Pomdog/Experimental/ImageEffects/SepiaToneEffect.hpp>
#include <Pomdog/Experimental/ImageEffects/VignetteEffect.hpp>
#include <random>

namespace FeatureShowcase {

ImageEffectsTest::ImageEffectsTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
    , postProcessCompositor(gameHostIn->GetGraphicsDevice())
{
}

void ImageEffectsTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);

    auto fxaa = std::make_shared<FXAA>(graphicsDevice, *assets);
    auto fishEyeEffect =
        std::make_shared<FishEyeEffect>(graphicsDevice, *assets);
    auto vignetteEffect =
        std::make_shared<VignetteEffect>(graphicsDevice, *assets);
    auto chromaticAberration =
        std::make_shared<ChromaticAberration>(graphicsDevice, *assets);
    auto sepiaToneEffect =
        std::make_shared<SepiaToneEffect>(graphicsDevice, *assets);
    auto retroCrtEffect =
        std::make_shared<RetroCrtEffect>(graphicsDevice, *assets);

    vignetteEffect->SetIntensity(1.0f);
    fishEyeEffect->SetStrength(0.3f);

    auto presentationParameters = graphicsDevice->GetPresentationParameters();
    renderTarget = std::make_shared<RenderTarget2D>(
        graphicsDevice, presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight, false,
        presentationParameters.BackBufferFormat,
        presentationParameters.DepthStencilFormat);

    postProcessCompositor.SetViewportSize(
        *graphicsDevice, presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        presentationParameters.DepthStencilFormat);

    postProcessCompositor.Composite({
        fishEyeEffect,
        fxaa,
        sepiaToneEffect,
        vignetteEffect,
        retroCrtEffect,
        chromaticAberration,
    });

    auto window = gameHost->GetWindow();

    connect(window->ClientSizeChanged, [this](int width, int height) {
        auto presentationParameters =
            graphicsDevice->GetPresentationParameters();
        renderTarget = std::make_shared<RenderTarget2D>(
            graphicsDevice, width, height, false,
            presentationParameters.BackBufferFormat,
            presentationParameters.DepthStencilFormat);

        postProcessCompositor.SetViewportSize(
            *graphicsDevice, width, height,
            presentationParameters.DepthStencilFormat);
    });
}

void ImageEffectsTest::Update() {}

void ImageEffectsTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth,
                         presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {renderTarget, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    primitiveBatch->Begin(commandList, projectionMatrix);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    primitiveBatch->DrawRectangle(Vector2::Zero, 100, 40, Vector2{1.0f, 1.0f}, Color::White);
    primitiveBatch->DrawRectangle(Vector2::Zero, 40, 100, Vector2{0.0f, 0.0f}, Color::Black);
    primitiveBatch->DrawRectangle(Vector2::Zero, 30, 30, Vector2{0.5f, 0.5f}, Color::Green);

    // Drawing triangle
    primitiveBatch->DrawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::Black, Color::Green, Color::Red);

    primitiveBatch->End();

    postProcessCompositor.Draw(*commandList, renderTarget);

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
