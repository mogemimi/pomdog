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
GamepadTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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

    return nullptr;
}

void GamepadTest::update()
{
}

void GamepadTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
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

    auto gamepadService = gameHost_->getGamepadService();

    auto printButton = [&](const std::string& name, GamepadButtons button, const std::shared_ptr<Gamepad>& pad) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, name + ":", textPos, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        if (pad->isButtonDown(button)) {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Press", pos, Color::createLime(), 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else {
            spriteFont_->draw(graphicsDevice_, *spriteBatch_, "Released", pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }

        textPos.y -= 18.0f;
    };

    auto printThumbstick = [&](const std::string& name, float s) {
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, name + ":", textPos, Color::createWhite(), 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        spriteFont_->draw(graphicsDevice_, *spriteBatch_, pomdog::format("{:.4f}", s), pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);

        textPos.y -= 18.0f;
    };

    auto printGamepad = [&](PlayerIndex index) {
        auto pad = gamepadService->getGamepad(index);

        if (!pad->isConnected()) {
            printText("Status", pomdog::format("Gamepad [{}] is not connected.", pomdog::to_underlying(index)));
            return;
        }

        printText("Name", std::string(pad->getName()));

        printButton("A", GamepadButtons::A, pad);
        printButton("B", GamepadButtons::B, pad);
        printButton("X", GamepadButtons::X, pad);
        printButton("Y", GamepadButtons::Y, pad);

        printButton("LeftMenu", GamepadButtons::LeftMenu, pad);
        printButton("RightMenu", GamepadButtons::RightMenu, pad);

        printButton("LeftTrigger", GamepadButtons::LeftTrigger, pad);
        printButton("RightTrigger", GamepadButtons::RightTrigger, pad);

        printButton("LeftStick", GamepadButtons::LeftStick, pad);
        printButton("RightStick", GamepadButtons::RightStick, pad);

        printButton("LeftBumper", GamepadButtons::LeftBumper, pad);
        printButton("RightBumper", GamepadButtons::RightBumper, pad);

        printButton("Guide", GamepadButtons::Guide, pad);
        printButton("Extra1", GamepadButtons::Extra1, pad);
        printButton("Extra2", GamepadButtons::Extra2, pad);

        auto leftStick = pad->getLeftStick();
        auto rightStick = pad->getRightStick();
        printThumbstick("ThumbSticks.Left.X", leftStick.x);
        printThumbstick("ThumbSticks.Left.Y", leftStick.y);
        printThumbstick("ThumbSticks.Right.X", rightStick.x);
        printThumbstick("ThumbSticks.Right.Y", rightStick.y);

        printButton("DPad.Up", GamepadButtons::DPadUp, pad);
        printButton("DPad.Down", GamepadButtons::DPadDown, pad);
        printButton("DPad.Right", GamepadButtons::DPadRight, pad);
        printButton("DPad.Left", GamepadButtons::DPadLeft, pad);
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
