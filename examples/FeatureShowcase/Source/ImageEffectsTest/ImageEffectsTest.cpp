#include "ImageEffectsTest.hpp"
#include <pomdog/experimental/image_effects/chromatic_aberration.hpp>
#include <pomdog/experimental/image_effects/fxaa.hpp>
#include <pomdog/experimental/image_effects/fish_eye_effect.hpp>
#include <pomdog/experimental/image_effects/retro_crt_effect.hpp>
#include <pomdog/experimental/image_effects/sepia_tone_effect.hpp>
#include <pomdog/experimental/image_effects/vignette_effect.hpp>
#include <random>

namespace FeatureShowcase {

ImageEffectsTest::ImageEffectsTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
    , postProcessCompositor(gameHostIn->GetGraphicsDevice())
{
}

std::unique_ptr<Error> ImageEffectsTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

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

    // NOTE: Create render target
    std::tie(renderTarget, err) = graphicsDevice->CreateRenderTarget2D(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        false,
        presentationParameters.BackBufferFormat);
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create depth stencil buffer
    std::tie(depthStencilBuffer, err) = graphicsDevice->CreateDepthStencilBuffer(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        presentationParameters.DepthStencilFormat);
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create depth stencil buffer");
    }

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
        auto presentationParameters = graphicsDevice->GetPresentationParameters();
        renderTarget = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            presentationParameters.BackBufferFormat));

        depthStencilBuffer = std::get<0>(graphicsDevice->CreateDepthStencilBuffer(
            width,
            height,
            presentationParameters.DepthStencilFormat));

        postProcessCompositor.SetViewportSize(
            *graphicsDevice, width, height,
            presentationParameters.DepthStencilFormat);
    });

    return nullptr;
}

void ImageEffectsTest::Update() {}

void ImageEffectsTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {renderTarget, Color::CornflowerBlue.ToVector4()};
    pass.DepthStencilBuffer = depthStencilBuffer;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
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
    primitiveBatch->DrawRectangle(Vector2::Zero(), 100, 40, Vector2{1.0f, 1.0f}, Color::White);
    primitiveBatch->DrawRectangle(Vector2::Zero(), 40, 100, Vector2{0.0f, 0.0f}, Color::Black);
    primitiveBatch->DrawRectangle(Vector2::Zero(), 30, 30, Vector2{0.5f, 0.5f}, Color::Green);

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
