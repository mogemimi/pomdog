#include "DistanceFieldFontTest.hpp"
#include <Pomdog/Experimental/Graphics/SpriteFontLoader.hpp>

namespace FeatureShowcase {

DistanceFieldFontTest::DistanceFieldFontTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> DistanceFieldFontTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        std::nullopt,
        SamplerDescription::CreateLinearWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::DistanceField,
        *assets);

    std::tie(spriteFont, err) = assets->Load<SpriteFont>("BitmapFonts/Ubuntu-Regular.fnt");
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load a font file");
    }

    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    return nullptr;
}

void DistanceFieldFontTest::Update()
{
}

void DistanceFieldFontTest::Draw()
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

    constexpr auto text = "Hello, world!\n0123456789\nABCDEFghijk";

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    const auto size = spriteFont->MeasureString(text);
    primitiveBatch->DrawLine(Vector2::Zero, Vector2{0.0f, 1.0f} * size, Color::Blue, 1.0f);
    primitiveBatch->DrawLine(Vector2::Zero, Vector2{1.0f, 0.0f} * size, Color::Blue, 1.0f);
    primitiveBatch->DrawLine(size, Vector2{0.0f, 1.0f} * size, Color::Blue, 1.0f);
    primitiveBatch->DrawLine(size, Vector2{1.0f, 0.0f} * size, Color::Blue, 1.0f);

    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::White, 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::Green, Math::ToRadians(-90.0f), Vector2{0.0f, 0.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, text, Vector2::Zero, Color::Red, Math::ToRadians(90.0f), Vector2{0.5f, 0.0f}, Vector2{-1.0f, 0.5f});
    spriteFont->Draw(*spriteBatch, text, Vector2{-100.0f, 100.0f}, Color::Blue, Math::ToRadians(-45.0f), Vector2{0.5f, 0.5f}, 0.7f);
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
