#include "gamepad_test.h"
#include "pomdog/utility/enum_cast.h"
#include "pomdog/utility/string_format.h"

namespace feature_showcase {

GamepadTest::GamepadTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
GamepadTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            std::nullopt,
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceField);
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

    constexpr bool useSDF = true;

    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 24.0f, 24.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }

    connect_(gameHost_->getGamepad()->Connected, [](PlayerIndex playerIndex, const GamepadCapabilities& caps) {
        Log::Verbose("Connected: " + caps.name + " at " + std::to_string(static_cast<int>(playerIndex)));
    });

    connect_(gameHost_->getGamepad()->Disconnected, [](PlayerIndex playerIndex, const GamepadCapabilities& caps) {
        Log::Verbose("Disconnected: " + caps.name + " at " + std::to_string(static_cast<int>(playerIndex)));
    });

    return nullptr;
}

void GamepadTest::update()
{
}

void GamepadTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

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

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    auto textPos = Vector2{-240.0f, 220.0f};
    constexpr float fontScale = 0.7f;
    auto printText = [&](const std::string& name, const std::string& s) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, name + ":", textPos, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, s, textPos + Vector2{10.0f, 0.0f}, Color::createYellow(), 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        textPos.y -= 18.0f;
    };

    auto gamepad = gameHost_->getGamepad();

    auto printButton = [&](const std::string& name, ButtonState button, bool hasButton) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, name + ":", textPos, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        if (!hasButton) {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Disabled", pos, Color::createRed(), 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else if (button == ButtonState::Down) {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Press", pos, Color::createLime(), 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Released", pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }

        textPos.y -= 18.0f;
    };

    auto printThumbstick = [&](const std::string& name, float s, bool hasButton) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, name + ":", textPos, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        if (!hasButton) {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Disabled", pos, Color::createRed(), 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, pomdog::format("{:.4f}", s), pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }

        textPos.y -= 18.0f;
    };

    auto printGamepad = [&](PlayerIndex index) {
        auto state = gamepad->getState(index);

        if (!state.isConnected) {
            printText("Status", pomdog::format("Gamepad [{}] is not connected.", pomdog::to_underlying(index)));
            return;
        }

        auto caps = gamepad->getCapabilities(index);
        printText("Name", caps.name);
        printText("DeviceUUID", caps.deviceUUID.toString());

        printButton("A", state.buttons.a, caps.hasAButton);
        printButton("B", state.buttons.b, caps.hasBButton);
        printButton("X", state.buttons.x, caps.hasXButton);
        printButton("Y", state.buttons.y, caps.hasYButton);

        printButton("LeftMenu", state.buttons.leftMenu, caps.hasLeftMenuButton);
        printButton("RightMenu", state.buttons.rightMenu, caps.hasRightMenuButton);

        printButton("LeftTrigger", state.buttons.leftTrigger, caps.hasLeftTrigger);
        printButton("RightTrigger", state.buttons.rightTrigger, caps.hasRightTrigger);

        printButton("LeftStick", state.buttons.leftStick, caps.hasLeftStickButton);
        printButton("RightStick", state.buttons.rightStick, caps.hasRightStickButton);

        printButton("LeftShoulder", state.buttons.leftShoulder, caps.hasLeftShoulderButton);
        printButton("RightShoulder", state.buttons.rightShoulder, caps.hasRightShoulderButton);

        printButton("Guide", state.buttons.guide, caps.hasGuideButton);
        printButton("Extra1", state.buttons.extra1, caps.hasExtra1Button);
        printButton("Extra2", state.buttons.extra2, caps.hasExtra2Button);

        printThumbstick("ThumbSticks.Left.X", state.thumbSticks.left.x, caps.hasLeftXThumbStick);
        printThumbstick("ThumbSticks.Left.Y", state.thumbSticks.left.y, caps.hasLeftYThumbStick);
        printThumbstick("ThumbSticks.Right.X", state.thumbSticks.right.x, caps.hasRightXThumbStick);
        printThumbstick("ThumbSticks.Right.Y", state.thumbSticks.right.y, caps.hasRightYThumbStick);

        printButton("DPad.Up", state.dpad.up, true);
        printButton("DPad.Down", state.dpad.down, true);
        printButton("DPad.Right", state.dpad.right, true);
        printButton("DPad.Left", state.dpad.left, true);
    };

    printGamepad(PlayerIndex::One);

    textPos = Vector2{100.0f, 220.0f};
    printGamepad(PlayerIndex::Two);

    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

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
