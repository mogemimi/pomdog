#include "image_effects_test.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/toggle_switch.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/grayscale_effect.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/sepia_tone_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"

namespace feature_showcase {

ImageEffectsTest::ImageEffectsTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
ImageEffectsTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice);

    if (auto initErr = postProcessCompositor.initialize(graphicsDevice); initErr != nullptr) {
        return errors::wrap(std::move(initErr), "failed to initialize post process compositor");
    }

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
    auto grayscaleEffect = std::make_shared<GrayscaleEffect>();
    if (auto grayErr = grayscaleEffect->initialize(fs_, graphicsDevice); grayErr != nullptr) {
        return errors::wrap(std::move(grayErr), "failed to initialize GrayscaleEffect");
    }

    vignetteEffect->setIntensity(1.0f);
    fishEyeEffect->setStrength(0.3f);

    // NOTE: Register effects with toggle state
    effectEntries_ = {
        {"FXAA", fxaa, true},
        {"Fish Eye", fishEyeEffect, true},
        {"Vignette", vignetteEffect, true},
        {"Chromatic Aberration", chromaticAberration, true},
        {"Sepia Tone", sepiaToneEffect, false},
        {"Retro CRT", retroCrtEffect, true},
        {"Grayscale", grayscaleEffect, false},
    };

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

    if (auto viewportErr = postProcessCompositor.setViewportSize(
            *graphicsDevice,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);
        viewportErr != nullptr) {
        return errors::wrap(std::move(viewportErr), "failed to set viewport size");
    }

    rebuildComposite();

    // NOTE: Setup GUI
    drawingContext_ = std::make_unique<gui::DrawingContext>(graphicsDevice, fs_);

    auto window = gameHost->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost->getKeyboard());

    auto dispatcher = hierarchy_->GetDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 200, 180);
        stackPanel->SetPosition(Point2D{5, 260});
        stackPanel->SetPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->AddChild(stackPanel);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 196, 180);
        verticalLayout->SetLayoutSpacing(4);
        stackPanel->AddChild(verticalLayout);

        for (std::size_t i = 0; i < effectEntries_.size(); ++i) {
            auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->AddChild(horizontalLayout);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->SetColor(Color::createWhite());
            textBlock->SetText(effectEntries_[i].name);
            horizontalLayout->AddChild(textBlock);

            auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggleSwitch->SetOn(effectEntries_[i].enabled);
            connect(toggleSwitch->Toggled, [this, index = i, toggleSwitch = toggleSwitch.get()](bool isOn) {
                if (!isOn) {
                    const auto enabledCount = std::count_if(effectEntries_.begin(), effectEntries_.end(), [](const EffectEntry& entry) {
                        return entry.enabled;
                    });
                    if (enabledCount == 1) {
                        toggleSwitch->SetOn(true);
                        return;
                    }
                }

                effectEntries_[index].enabled = isOn;
                rebuildComposite();
            });
            horizontalLayout->AddChild(toggleSwitch);
        }
    }

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

        // NOTE: Ignore errors in resize callback
        [[maybe_unused]] auto err = postProcessCompositor.setViewportSize(
            *graphicsDevice, width, height,
            presentationParameters.depthStencilFormat);
    });

    return nullptr;
}

void ImageEffectsTest::rebuildComposite()
{
    std::vector<std::shared_ptr<ImageEffectBase>> activeEffects;
    for (const auto& entry : effectEntries_) {
        if (entry.enabled) {
            activeEffects.push_back(entry.effect);
        }
    }
    postProcessCompositor.composite(std::move(activeEffects));
}

void ImageEffectsTest::update()
{
    hierarchy_->Update();

    if (auto mouse = gameHost->getMouse(); mouse != nullptr) {
        hierarchy_->Touch(mouse->getState());
    }

    auto clock = gameHost->getClock();
    hierarchy_->UpdateAnimation(clock->getFrameDuration());
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

    // NOTE: Draw GUI overlay
    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->Reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
    drawingContext_->BeginDraw(commandList, viewMatrix * projectionMatrix);
    hierarchy_->Draw(*drawingContext_);
    drawingContext_->EndDraw();

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
