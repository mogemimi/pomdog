#include "sprite_font_test.h"

namespace feature_showcase {

SpriteFontTest::SpriteFontTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> SpriteFontTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::Wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 32.0f, 32.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    return nullptr;
}

void SpriteFontTest::Update()
{
}

void SpriteFontTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

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
    pass.scissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    constexpr auto text = "Hello, world!\n0123456789\nABCDEFghijk";

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    const auto size = spriteFont->MeasureString(text);
    primitiveBatch->DrawLine(Vector2::createZero(), Vector2{0.0f, 1.0f} * size, Color::createBlue(), 1.0f);
    primitiveBatch->DrawLine(Vector2::createZero(), Vector2{1.0f, 0.0f} * size, Color::createBlue(), 1.0f);
    primitiveBatch->DrawLine(size, Vector2{0.0f, 1.0f} * size, Color::createBlue(), 1.0f);
    primitiveBatch->DrawLine(size, Vector2{1.0f, 0.0f} * size, Color::createBlue(), 1.0f);

    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, text, Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::createZero(), Color::createGreen(), math::toRadians(-90.0f), Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::createZero(), Color::createRed(), math::toRadians(90.0f), Vector2{0.5f, 0.0f}, Vector2{-1.0f, 0.5f});
    spriteFont->Draw(*spriteBatch, text, Vector2{-100.0f, 100.0f}, Color::createBlue(), math::toRadians(-45.0f), Vector2{0.5f, 0.5f}, 0.7f);
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

} // namespace feature_showcase
