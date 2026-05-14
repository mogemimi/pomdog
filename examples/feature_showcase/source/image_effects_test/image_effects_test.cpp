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
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
ImageEffectsTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }

    if (auto err = postProcessCompositor_.initialize(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize post process compositor");
    }

    auto fxaa = std::make_shared<FXAA>();
    if (auto err = fxaa->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize FXAA");
    }
    auto fishEyeEffect = std::make_shared<FishEyeEffect>();
    if (auto err = fishEyeEffect->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize FishEyeEffect");
    }
    auto vignetteEffect = std::make_shared<VignetteEffect>();
    if (auto err = vignetteEffect->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize VignetteEffect");
    }
    auto chromaticAberration = std::make_shared<ChromaticAberration>();
    if (auto err = chromaticAberration->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize ChromaticAberration");
    }
    auto sepiaToneEffect = std::make_shared<SepiaToneEffect>();
    if (auto err = sepiaToneEffect->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize SepiaToneEffect");
    }
    auto retroCrtEffect = std::make_shared<RetroCrtEffect>();
    if (auto err = retroCrtEffect->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize RetroCrtEffect");
    }
    auto grayscaleEffect = std::make_shared<GrayscaleEffect>();
    if (auto err = grayscaleEffect->initialize(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize GrayscaleEffect");
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

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: Create render target
    if (auto [renderTarget, err] = graphicsDevice_->createRenderTarget2D(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            false,
            presentationParameters.backBufferFormat);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create render target");
    }
    else {
        renderTarget_ = std::move(renderTarget);
    }

    // NOTE: Create depth stencil buffer
    if (auto [depthStencilBuffer, err] = graphicsDevice_->createDepthStencilBuffer(
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create depth stencil buffer");
    }
    else {
        depthStencilBuffer_ = std::move(depthStencilBuffer);
    }

    if (auto viewportErr = postProcessCompositor_.setViewportSize(
            *graphicsDevice_,
            presentationParameters.backBufferWidth,
            presentationParameters.backBufferHeight,
            presentationParameters.depthStencilFormat);
        viewportErr != nullptr) {
        return errors::wrap(std::move(viewportErr), "failed to set viewport size");
    }

    rebuildComposite();

    // NOTE: Setup GUI
    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto err = drawingContext_->initialize(graphicsDevice_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 200, 180);
        stackPanel->setPosition(Point2D{5, 260});
        stackPanel->setPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->addChild(stackPanel);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 196, 180);
        verticalLayout->setLayoutSpacing(4);
        stackPanel->addChild(verticalLayout);

        for (std::size_t i = 0; i < effectEntries_.size(); ++i) {
            auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(horizontalLayout);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText(effectEntries_[i].name);
            horizontalLayout->addChild(textBlock);

            auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggleSwitch->setOn(effectEntries_[i].enabled);
            connect_(toggleSwitch->Toggled, [this, index = i, toggleSwitch = toggleSwitch.get()](bool isOn) {
                if (!isOn) {
                    const auto enabledCount = std::count_if(effectEntries_.begin(), effectEntries_.end(), [](const EffectEntry& entry) {
                        return entry.enabled;
                    });
                    if (enabledCount == 1) {
                        toggleSwitch->setOn(true);
                        return;
                    }
                }

                effectEntries_[index].enabled = isOn;
                rebuildComposite();
            });
            horizontalLayout->addChild(toggleSwitch);
        }
    }

    connect_(window->clientSizeChanged, [this](int width, int height) {
        auto presentationParameters = graphicsDevice_->getPresentationParameters();
        renderTarget_ = std::get<0>(graphicsDevice_->createRenderTarget2D(
            width,
            height,
            false,
            presentationParameters.backBufferFormat));

        depthStencilBuffer_ = std::get<0>(graphicsDevice_->createDepthStencilBuffer(
            width,
            height,
            presentationParameters.depthStencilFormat));

        // NOTE: Ignore errors in resize callback
        [[maybe_unused]] auto err = postProcessCompositor_.setViewportSize(
            *graphicsDevice_, width, height,
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
    postProcessCompositor_.composite(std::move(activeEffects));
}

void ImageEffectsTest::update()
{
    hierarchy_->update();

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(*mouse);
    }

    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void ImageEffectsTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {renderTarget_, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = depthStencilBuffer_;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);

    // Drawing line
    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    primitiveBatch_->drawRectangle(Vector2::createZero(), 100, 40, Vector2{1.0f, 1.0f}, Color::createWhite());
    primitiveBatch_->drawRectangle(Vector2::createZero(), 40, 100, Vector2{0.0f, 0.0f}, Color::createBlack());
    primitiveBatch_->drawRectangle(Vector2::createZero(), 30, 30, Vector2{0.5f, 0.5f}, Color::createLime());

    // Drawing triangle
    primitiveBatch_->drawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::createBlack(), Color::createLime(), Color::createRed());

    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();

    postProcessCompositor_.draw(*commandList_, renderTarget_);

    // NOTE: Draw GUI overlay
    {
        gpu::RenderPass guiPass;
        guiPass.renderTargets[0] = {nullptr, std::nullopt};
        guiPass.viewport = viewport;
        guiPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(guiPass));
    }
    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<f32>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
    drawingContext_->beginDraw(commandList_, viewMatrix * projectionMatrix);
    hierarchy_->draw(*drawingContext_);
    drawingContext_->endDraw();

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
