#include "image_effects_test.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/sepia_tone_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"
#include <random>

namespace feature_showcase {

ImageEffectsTest::ImageEffectsTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
    , postProcessCompositor(gameHostIn->getGraphicsDevice())
{
}

std::unique_ptr<Error> ImageEffectsTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
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
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        presentationParameters.backBufferFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create depth stencil buffer
    std::tie(depthStencilBuffer, err) = graphicsDevice->CreateDepthStencilBuffer(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }

    postProcessCompositor.SetViewportSize(
        *graphicsDevice, presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);

    postProcessCompositor.Composite({
        fishEyeEffect,
        fxaa,
        sepiaToneEffect,
        vignetteEffect,
        retroCrtEffect,
        chromaticAberration,
    });

    auto window = gameHost->getWindow();

    connect(window->clientSizeChanged, [this](int width, int height) {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();
        renderTarget = std::get<0>(graphicsDevice->CreateRenderTarget2D(
            width,
            height,
            false,
            presentationParameters.backBufferFormat));

        depthStencilBuffer = std::get<0>(graphicsDevice->CreateDepthStencilBuffer(
            width,
            height,
            presentationParameters.depthStencilFormat));

        postProcessCompositor.SetViewportSize(
            *graphicsDevice, width, height,
            presentationParameters.depthStencilFormat);
    });

    return nullptr;
}

void ImageEffectsTest::update()
{
}

void ImageEffectsTest::draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {renderTarget, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = depthStencilBuffer;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    primitiveBatch->Begin(commandList, projectionMatrix);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    primitiveBatch->DrawRectangle(Vector2::createZero(), 100, 40, Vector2{1.0f, 1.0f}, Color::createWhite());
    primitiveBatch->DrawRectangle(Vector2::createZero(), 40, 100, Vector2{0.0f, 0.0f}, Color::createBlack());
    primitiveBatch->DrawRectangle(Vector2::createZero(), 30, 30, Vector2{0.5f, 0.5f}, Color::createGreen());

    // Drawing triangle
    primitiveBatch->DrawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::createBlack(), Color::createGreen(), Color::createRed());

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

} // namespace feature_showcase
