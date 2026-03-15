#include "image_effects_test.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/sepia_tone_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

ImageEffectsTest::ImageEffectsTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
    , postProcessCompositor(gameHostIn->getGraphicsDevice())
{
}

std::unique_ptr<Error>
ImageEffectsTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice);

    auto fxaa = std::make_shared<FXAA>();
    if (auto fxaaErr = fxaa->initialize(fs_, graphicsDevice); fxaaErr != nullptr) {
        return errors::wrap(std::move(fxaaErr), "failed to initialize FXAA");
    }
    auto fishEyeEffect = std::make_shared<FishEyeEffect>();
    if (auto fishErr = fishEyeEffect->initialize(fs_, graphicsDevice); fishErr != nullptr) {
        return errors::wrap(std::move(fishErr), "failed to initialize FishEyeEffect");
    }
    auto vignetteEffect = std::make_shared<VignetteEffect>();
    if (auto vigErr = vignetteEffect->initialize(fs_, graphicsDevice); vigErr != nullptr) {
        return errors::wrap(std::move(vigErr), "failed to initialize VignetteEffect");
    }
    auto chromaticAberration = std::make_shared<ChromaticAberration>();
    if (auto caErr = chromaticAberration->initialize(fs_, graphicsDevice); caErr != nullptr) {
        return errors::wrap(std::move(caErr), "failed to initialize ChromaticAberration");
    }
    auto sepiaToneEffect = std::make_shared<SepiaToneEffect>();
    if (auto sepiaErr = sepiaToneEffect->initialize(fs_, graphicsDevice); sepiaErr != nullptr) {
        return errors::wrap(std::move(sepiaErr), "failed to initialize SepiaToneEffect");
    }
    auto retroCrtEffect = std::make_shared<RetroCrtEffect>();
    if (auto retroErr = retroCrtEffect->initialize(fs_, graphicsDevice); retroErr != nullptr) {
        return errors::wrap(std::move(retroErr), "failed to initialize RetroCrtEffect");
    }

    vignetteEffect->setIntensity(1.0f);
    fishEyeEffect->setStrength(0.3f);

    auto presentationParameters = graphicsDevice->getPresentationParameters();

    // NOTE: Create render target
    std::tie(renderTarget, err) = graphicsDevice->createRenderTarget2D(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        false,
        presentationParameters.backBufferFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }

    // NOTE: Create depth stencil buffer
    std::tie(depthStencilBuffer, err) = graphicsDevice->createDepthStencilBuffer(
        presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }

    postProcessCompositor.setViewportSize(
        *graphicsDevice, presentationParameters.backBufferWidth,
        presentationParameters.backBufferHeight,
        presentationParameters.depthStencilFormat);

    postProcessCompositor.composite({
        fishEyeEffect,
        fxaa,
        sepiaToneEffect,
        vignetteEffect,
        retroCrtEffect,
        chromaticAberration,
    });

    auto window = gameHost->getWindow();

    connect(window->clientSizeChanged, [this](int width, int height) {
        auto presentationParameters = graphicsDevice->getPresentationParameters();
        renderTarget = std::get<0>(graphicsDevice->createRenderTarget2D(
            width,
            height,
            false,
            presentationParameters.backBufferFormat));

        depthStencilBuffer = std::get<0>(graphicsDevice->createDepthStencilBuffer(
            width,
            height,
            presentationParameters.depthStencilFormat));

        postProcessCompositor.setViewportSize(
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
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {renderTarget, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = depthStencilBuffer;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList->reset();
    commandList->setRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    primitiveBatch->begin(commandList, projectionMatrix);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    primitiveBatch->drawRectangle(Vector2::createZero(), 100, 40, Vector2{1.0f, 1.0f}, Color::createWhite());
    primitiveBatch->drawRectangle(Vector2::createZero(), 40, 100, Vector2{0.0f, 0.0f}, Color::createBlack());
    primitiveBatch->drawRectangle(Vector2::createZero(), 30, 30, Vector2{0.5f, 0.5f}, Color::createLime());

    // Drawing triangle
    primitiveBatch->drawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::createBlack(), Color::createLime(), Color::createRed());

    primitiveBatch->end();

    postProcessCompositor.draw(*commandList, renderTarget);

    commandList->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->reset();
        commandQueue->pushBackCommandList(commandList);
        commandQueue->executeCommandLists();
        commandQueue->present();
    }
    else {
        commandQueue->pushBackCommandList(commandList);
    }
}

} // namespace feature_showcase
