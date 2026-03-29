#include "sprite_line_test.h"
#include "pomdog/experimental/graphics/sprite_line.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

SpriteLineTest::SpriteLineTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost(gameHostIn)
    , fs_(fs)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
SpriteLineTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>();
    if (auto primitiveBatchErr = primitiveBatch->initialize(fs_, graphicsDevice); primitiveBatchErr != nullptr) {
        return errors::wrap(std::move(primitiveBatchErr), "failed to initialize PrimitiveBatch");
    }
    spriteBatch = std::make_shared<SpriteBatch>();
    if (auto spriteBatchErr = spriteBatch->initialize(
            fs_,
            graphicsDevice,
            gpu::BlendDesc::createAlphaBlend(),
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Default);
        spriteBatchErr != nullptr) {
        return errors::wrap(std::move(spriteBatchErr), "failed to initialize SpriteBatch");
    }

    // NOTE: Load texture from PNG image file.
    std::tie(texture, err) = loadTexture2D(fs_, graphicsDevice, "/assets/textures/pomdog.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    mousePosition = Vector2{100.0f, 100.0f};

    return nullptr;
}

void SpriteLineTest::update()
{
    const auto mouse = gameHost->getMouse();
    const auto window = gameHost->getWindow();
    const auto clientBounds = window->getClientBounds();
    auto pos = mouse->getState().position;

    if (mouse->getState().leftButton == ButtonState::Down) {
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);

        mousePosition = math::toVector2(pos);
    }
}

void SpriteLineTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList->reset();
    commandList->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch->begin(commandList, projectionMatrix);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->end();

    spriteBatch->begin(commandList, projectionMatrix);

    SpriteLine spriteLine;
    spriteLine.draw(
        *spriteBatch,
        texture,
        Rect2D{0, 0, 10, 32},
        Rect2D{10, 0, 12, 32},
        Rect2D{22, 0, 10, 32},
        Vector2{0.0f, 0.0f},
        mousePosition,
        1.0f,
        Color::createWhite());

    spriteBatch->end();

    commandList->endRenderPass();
    commandList->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->reset();
        commandQueue->pushBackCommandList(commandList);
        commandQueue->executeCommandLists();
        commandQueue->present();
    }
    else {
        commandQueue->pushBackCommandList(commandList);
    }
}

} // namespace feature_showcase
