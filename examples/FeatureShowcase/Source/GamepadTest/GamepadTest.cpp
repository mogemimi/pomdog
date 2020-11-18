#include "GamepadTest.hpp"

namespace FeatureShowcase {

GamepadTest::GamepadTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void GamepadTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::get<0>(graphicsDevice->CreateGraphicsCommandList());
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->ToString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 24.0f, 24.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    connect(gameHost->GetGamepad()->Connected, [](PlayerIndex playerIndex, const GamepadCapabilities& caps) {
        Log::Verbose("Connected: " + caps.Name + " at " + std::to_string(static_cast<int>(playerIndex)));
    });

    connect(gameHost->GetGamepad()->Disconnected, [](PlayerIndex playerIndex, const GamepadCapabilities& caps) {
        Log::Verbose("Disconnected: " + caps.Name + " at " + std::to_string(static_cast<int>(playerIndex)));
    });
}

void GamepadTest::Update()
{
}

void GamepadTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    spriteBatch->Begin(commandList, projectionMatrix);
    auto textPos = Vector2{-240.0f, 220.0f};
    constexpr float fontScale = 0.7f;
    auto printText = [&](const std::string& name, const std::string& s) {
        spriteFont->Draw(*spriteBatch, name + ":", textPos, Color::White, 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        spriteFont->Draw(*spriteBatch, s, textPos + Vector2{10.0f, 0.0f}, Color::Yellow, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        textPos.Y -= 18.0f;
    };

    auto gamepad = gameHost->GetGamepad();

    auto printButton = [&](const std::string& name, ButtonState button, bool hasButton) {
        spriteFont->Draw(*spriteBatch, name + ":", textPos, Color::White, 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        if (!hasButton) {
            spriteFont->Draw(*spriteBatch, "Disabled", pos, Color::Red, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else if (button == ButtonState::Pressed) {
            spriteFont->Draw(*spriteBatch, "Press", pos, Color::Green, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else {
            spriteFont->Draw(*spriteBatch, "Released", pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }

        textPos.Y -= 18.0f;
    };

    auto printThumbstick = [&](const std::string& name, float s, bool hasButton) {
        spriteFont->Draw(*spriteBatch, name + ":", textPos, Color::White, 0.0f, Vector2{1.0f, 0.5f}, fontScale);
        auto pos = textPos + Vector2{10.0f, 0.0f};
        if (!hasButton) {
            spriteFont->Draw(*spriteBatch, "Disabled", pos, Color::Red, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }
        else {
            spriteFont->Draw(*spriteBatch, StringHelper::Format("%.4f", s), pos, Color{0, 255, 255, 120}, 0.0f, Vector2{0.0f, 0.5f}, fontScale);
        }

        textPos.Y -= 18.0f;
    };

    auto printGamepad = [&](PlayerIndex index) {
        auto state = gamepad->GetState(index);

        if (!state.IsConnected) {
            printText("Status", StringHelper::Format("Gamepad [%d] is not connected.", static_cast<int>(index)));
            return;
        }

        auto caps = gamepad->GetCapabilities(index);
        printText("Name", caps.Name);
        printText("DeviceUUID", caps.DeviceUUID.ToString());

        printButton("A", state.Buttons.A, caps.HasAButton);
        printButton("B", state.Buttons.B, caps.HasBButton);
        printButton("X", state.Buttons.X, caps.HasXButton);
        printButton("Y", state.Buttons.Y, caps.HasYButton);

        printButton("LeftMenu", state.Buttons.LeftMenu, caps.HasLeftMenuButton);
        printButton("RightMenu", state.Buttons.RightMenu, caps.HasRightMenuButton);

        printButton("LeftTrigger", state.Buttons.LeftTrigger, caps.HasLeftTrigger);
        printButton("RightTrigger", state.Buttons.RightTrigger, caps.HasRightTrigger);

        printButton("LeftStick", state.Buttons.LeftStick, caps.HasLeftStickButton);
        printButton("RightStick", state.Buttons.RightStick, caps.HasRightStickButton);

        printButton("LeftShoulder", state.Buttons.LeftShoulder, caps.HasLeftShoulderButton);
        printButton("RightShoulder", state.Buttons.RightShoulder, caps.HasRightShoulderButton);

        printButton("Guide", state.Buttons.Guide, caps.HasGuideButton);
        printButton("Extra1", state.Buttons.Extra1, caps.HasExtra1Button);
        printButton("Extra2", state.Buttons.Extra2, caps.HasExtra2Button);

        printThumbstick("ThumbSticks.Left.X", state.ThumbSticks.Left.X, caps.HasLeftXThumbStick);
        printThumbstick("ThumbSticks.Left.Y", state.ThumbSticks.Left.Y, caps.HasLeftYThumbStick);
        printThumbstick("ThumbSticks.Right.X", state.ThumbSticks.Right.X, caps.HasRightXThumbStick);
        printThumbstick("ThumbSticks.Right.Y", state.ThumbSticks.Right.Y, caps.HasRightYThumbStick);

        printButton("DPad.Up", state.DPad.Up, true);
        printButton("DPad.Down", state.DPad.Down, true);
        printButton("DPad.Right", state.DPad.Right, true);
        printButton("DPad.Left", state.DPad.Left, true);
    };

    printGamepad(PlayerIndex::One);

    textPos = Vector2{100.0f, 220.0f};
    printGamepad(PlayerIndex::Two);

    spriteBatch->End();

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
