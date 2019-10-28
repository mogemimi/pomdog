#include "SpriteLineTest.hpp"
#include <Pomdog/Experimental/Graphics/SpriteLine.hpp>
#include <random>

namespace FeatureShowcase {

SpriteLineTest::SpriteLineTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void SpriteLineTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        SamplerDescription::CreateLinearWrap(),
        std::nullopt,
        std::nullopt,
        *assets);

    std::shared_ptr<Error> err;
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }

    mousePosition = Vector2{100.0f, 100.0f};
}

void SpriteLineTest::Update()
{
    auto mouse = gameHost->GetMouse();
    auto window = gameHost->GetWindow();
    auto pos = mouse->GetState().Position;

    if (mouse->GetState().LeftButton == ButtonState::Pressed) {
        pos.X = pos.X - (window->GetClientBounds().Width / 2);
        pos.Y = -pos.Y + (window->GetClientBounds().Height / 2);

        mousePosition = MathHelper::ToVector2(pos);
    }
}

void SpriteLineTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

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
    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);

    SpriteLine spriteLine;
    spriteLine.Draw(*spriteBatch,
                    texture,
                    Rectangle{0, 0, 10, 32},
                    Rectangle{10, 0, 12, 32},
                    Rectangle{22, 0, 10, 32},
                    Vector2{0.0f, 0.0f},
                    mousePosition,
                    1.0f,
                    Color::White);

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
