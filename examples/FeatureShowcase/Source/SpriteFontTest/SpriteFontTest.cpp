#include "SpriteFontTest.hpp"

namespace FeatureShowcase {

SpriteFontTest::SpriteFontTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void SpriteFontTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    polygonBatch = std::make_shared<PolygonBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto font = std::make_shared<TrueTypeFont>(*assets, "Fonts/NotoSans/NotoSans-Regular.ttf");
    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 32, 32);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
}

void SpriteFontTest::Update()
{
}

void SpriteFontTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        0.0f,
        100.0f);

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    constexpr auto text = "Hello, world!\n0123456789\nABCDEFghijk";

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    polygonBatch->Begin(commandList, projectionMatrix);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    polygonBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    const auto size = spriteFont->MeasureString(text);
    polygonBatch->DrawLine(Vector2::Zero, Vector2{0.0f, 1.0f} * size, Color::Blue, 1.0f);
    polygonBatch->DrawLine(Vector2::Zero, Vector2{1.0f, 0.0f} * size, Color::Blue, 1.0f);
    polygonBatch->DrawLine(size, Vector2{0.0f, 1.0f} * size, Color::Blue, 1.0f);
    polygonBatch->DrawLine(size, Vector2{1.0f, 0.0f} * size, Color::Blue, 1.0f);

    polygonBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::White, 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::Green, MathHelper::ToRadians(-90.0f), Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::Red, MathHelper::ToRadians(90.0f), Vector2{0.5f, 0.0f}, Vector2{-1.0f, 0.5f});
    spriteFont->Draw(*spriteBatch, text, Vector2{-100.0f, 100.0f}, Color::Blue, MathHelper::ToRadians(-45.0f), Vector2{0.5f, 0.5f}, 0.7f);
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
