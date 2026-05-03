// Copyright mogemimi. Distributed under the MIT license.

#include "display_settings_test.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/toggle_switch.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/utility/string_format.h"

namespace feature_showcase {

DisplaySettingsTest::DisplaySettingsTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
DisplaySettingsTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    window_ = gameHost_->getWindow();

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
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceFieldWithOutline);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }
    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 20.0f, 20.0f, true); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }
    spriteFont_->prepareFonts(graphicsDevice_, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?/:()> ");

    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto err = drawingContext_->initialize(graphicsDevice_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DrawingContext");
    }

    auto keyboard = gameHost_->getKeyboard();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window_, keyboard);

    // NOTE: Connect the windowModeChanged signal to update statusText_.
    connect_(window_->windowModeChanged, [this](WindowMode mode) {
        switch (mode) {
        case WindowMode::Windowed:
            statusText_ = "Mode: Windowed";
            break;
        case WindowMode::BorderlessWindowed:
            statusText_ = "Mode: BorderlessWindowed";
            break;
        case WindowMode::Maximized:
            statusText_ = "Mode: Maximized";
            break;
        case WindowMode::Fullscreen:
            statusText_ = "Mode: Fullscreen";
            break;
        case WindowMode::BrowserSoftFullscreen:
            statusText_ = "Mode: BrowserSoftFullscreen";
            break;
        }
    });

    // NOTE: Set initial status text.
    switch (window_->getWindowMode()) {
    case WindowMode::Windowed:
        statusText_ = "Mode: Windowed";
        break;
    case WindowMode::BorderlessWindowed:
        statusText_ = "Mode: BorderlessWindowed";
        break;
    case WindowMode::Maximized:
        statusText_ = "Mode: Maximized";
        break;
    case WindowMode::Fullscreen:
        statusText_ = "Mode: Fullscreen";
        break;
    case WindowMode::BrowserSoftFullscreen:
        statusText_ = "Mode: BrowserSoftFullscreen";
        break;
    }

    auto dispatcher = hierarchy_->getDispatcher();

    // NOTE: Control panel
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 240, 270);
        stackPanel->setPosition(Point2D{5, 190});
        stackPanel->setPadding(gui::Thickness{2, 0, 2, 8});
        hierarchy_->addChild(stackPanel);

        {
            auto navigator = std::make_shared<gui::DebugNavigator>(dispatcher, gameHost_->getClock());
            stackPanel->addChild(navigator);
        }

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 220, 200);
        verticalLayout->setLayoutSpacing(6);
        stackPanel->addChild(verticalLayout);

        // NOTE: Section label - Window Mode
        {
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("Window Mode");
            verticalLayout->addChild(label);
        }

        // NOTE: Windowed button
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Windowed");
            connect_(button->Click, [this]() {
                applyWindowMode(WindowMode::Windowed);
            });
            verticalLayout->addChild(button);
        }

        // NOTE: Borderless Windowed button
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Borderless Windowed");
            connect_(button->Click, [this]() {
                applyWindowMode(WindowMode::BorderlessWindowed);
            });
            verticalLayout->addChild(button);
        }

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
        // NOTE: Maximized button (desktop only)
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Maximized");
            connect_(button->Click, [this]() {
                applyWindowMode(WindowMode::Maximized);
            });
            verticalLayout->addChild(button);
        }
#endif

        // NOTE: Fullscreen button
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Fullscreen");
            connect_(button->Click, [this]() {
                applyWindowMode(WindowMode::Fullscreen);
            });
            verticalLayout->addChild(button);
        }

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
        // NOTE: Browser Soft Fullscreen button (Emscripten only)
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Browser Soft Fullscreen");
            connect_(button->Click, [this]() {
                applyWindowMode(WindowMode::BrowserSoftFullscreen);
            });
            verticalLayout->addChild(button);
        }
#endif

        // NOTE: Separator label - Window Properties
        {
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("Window Properties");
            verticalLayout->addChild(label);
        }

        // NOTE: Allow user resizing toggle
        {
            auto layout = std::make_shared<gui::HorizontalLayout>(dispatcher, 220, 24);
            layout->setLayoutSpacing(4);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("Allow Resizing");
            layout->addChild(label);

            auto toggle = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggle->setOn(window_->getAllowUserResizing());
            connect_(toggle->Toggled, [this](bool isOn) {
                window_->requestAllowUserResizing(isOn);
            });
            layout->addChild(toggle);

            verticalLayout->addChild(layout);
        }

        // NOTE: setClientBounds button - resize to 800x600
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Client Bounds 800x600");
            connect_(button->Click, [this]() {
                [[maybe_unused]] auto err = window_->requestClientBounds(Rect2D{0, 0, 800, 600});
            });
            verticalLayout->addChild(button);
        }

        // NOTE: setTitle button - sets a title containing Japanese (UTF-8) to verify encoding fix
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Set Title (Japanese)");
            connect_(button->Click, [this]() {
                window_->requestTitle("DisplaySettings \xe3\x83\x86\xe3\x82\xb9\xe3\x83\x88");
            });
            verticalLayout->addChild(button);
        }

        // NOTE: clientSizeChanged signal test text label
        {
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("Resize the window to see clientSizeChanged signal in action.");
            connect_(window_->clientSizeChanged, [label](int width, int height) {
                label->setText(pomdog::format("Client Size Changed: {}x{}", width, height));
            });
            verticalLayout->addChild(label);
        }

        // NOTE: Display Settings section heading
        {
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("Display Settings");
            verticalLayout->addChild(label);
        }

        // NOTE: V-Sync toggle
        {
            auto layout = std::make_shared<gui::HorizontalLayout>(dispatcher, 220, 24);
            layout->setLayoutSpacing(4);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("V-Sync");
            layout->addChild(label);

            auto toggle = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggle->setOn(gameHost_->getDisplaySyncEnabled());
            connect_(toggle->Toggled, [this](bool isOn) {
                gameHost_->setDisplaySyncEnabled(isOn);
            });
            layout->addChild(toggle);

            verticalLayout->addChild(layout);
        }

        // NOTE: FPS cap toggle (30 fps cap on/off)
        {
            auto layout = std::make_shared<gui::HorizontalLayout>(dispatcher, 220, 24);
            layout->setLayoutSpacing(4);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color{200, 200, 200, 255});
            label->setText("FPS Cap");
            layout->addChild(label);

            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            popupMenu->addItem("No Cap");
            popupMenu->addItem("15");
            popupMenu->addItem("24");
            popupMenu->addItem("30");
            popupMenu->addItem("45");
            popupMenu->addItem("60");
            popupMenu->addItem("90");
            popupMenu->addItem("120");
            popupMenu->addItem("144");
            popupMenu->addItem("160");
            popupMenu->addItem("180");
            popupMenu->addItem("320");
            connect_(popupMenu->CurrentIndexChanged, [this](i32 index) {
                constexpr auto fpsOptions = std::array<std::optional<i32>, 12>{
                    std::nullopt, 15, 24, 30, 45, 60, 90, 120, 144, 160, 180, 320};

                if (index >= 0 && index < static_cast<i32>(fpsOptions.size())) {
                    gameHost_->setMaxFramesPerSecond(fpsOptions[index]);
                }
            });
            layout->addChild(popupMenu);

            verticalLayout->addChild(layout);
        }
    }

    return nullptr;
}

void DisplaySettingsTest::applyWindowMode(WindowMode mode) noexcept
{
    if (auto err = window_->requestWindowMode(mode); err != nullptr) {
        statusText_ = pomdog::format("Error: {}", err->toString());
    }
}

void DisplaySettingsTest::update()
{
    hierarchy_->update();

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(*mouse);
    }

    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void DisplaySettingsTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // NOTE: Draw background grid
    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();

    // NOTE: Draw GUI overlay
    {
        gpu::RenderPass guiPass;
        guiPass.renderTargets[0] = {nullptr, std::nullopt};
        guiPass.viewport = viewport;
        guiPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(guiPass));
    }

    const auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<f32>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    // NOTE: Draw status text (current window mode)
    if (!statusText_.empty()) {
        const auto statusPos = Vector2{
            static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f + 8.0f,
            static_cast<f32>(presentationParameters.backBufferHeight) * 0.5f - 28.0f,
        };
        spriteBatch_->reset();
        spriteBatch_->setTransform(viewMatrix * projectionMatrix);
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, statusText_, statusPos, Color::createWhite(), 0.0f, Vector2{0.0f, 0.5f}, 1.0f);
        spriteBatch_->flush(commandList_, spritePipeline_);
        spriteBatch_->submit(graphicsDevice_);
    }

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
