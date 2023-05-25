#include "sprite_line_test.h"
#include "pomdog/experimental/graphics/sprite_line.h"
#include <random>

namespace feature_showcase {

SpriteLineTest::SpriteLineTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> SpriteLineTest::Initialize()
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
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::CreateAlphaBlend(),
        std::nullopt,
        gpu::SamplerDescriptor::CreateLinearWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Load texture from PNG image file.
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    mousePosition = Vector2{100.0f, 100.0f};

    return nullptr;
}

void SpriteLineTest::Update()
{
    const auto mouse = gameHost->GetMouse();
    const auto window = gameHost->GetWindow();
    const auto clientBounds = window->GetClientBounds();
    auto pos = mouse->GetState().Position;

    if (mouse->GetState().LeftButton == ButtonState::Pressed) {
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);

        mousePosition = math::toVector2(pos);
    }
}

void SpriteLineTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

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

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

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
    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);

    SpriteLine spriteLine;
    spriteLine.Draw(
        *spriteBatch,
        texture,
        Rectangle{0, 0, 10, 32},
        Rectangle{10, 0, 12, 32},
        Rectangle{22, 0, 10, 32},
        Vector2{0.0f, 0.0f},
        mousePosition,
        1.0f,
        Color::createWhite());

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
