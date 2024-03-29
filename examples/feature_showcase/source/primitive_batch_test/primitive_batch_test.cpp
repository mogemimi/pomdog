#include "primitive_batch_test.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace feature_showcase {

PrimitiveBatchTest::PrimitiveBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> PrimitiveBatchTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->setInterval(std::chrono::seconds(1));

    return nullptr;
}

void PrimitiveBatchTest::update()
{
}

void PrimitiveBatchTest::draw()
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
    commandList->setRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    const auto t = static_cast<float>(timer->getTotalTime().count());

    primitiveBatch->begin(commandList, projectionMatrix);

    // Drawing line
    primitiveBatch->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);

    // Drawing rectangle
    primitiveBatch->drawRectangle(Vector2::createZero(), 100, 40, Vector2{1.0f, 1.0f}, Color::createWhite());
    primitiveBatch->drawRectangle(Vector2::createZero(), 40, 100, Vector2{0.0f, 0.0f}, Color::createBlack());
    primitiveBatch->drawRectangle(Vector2::createZero(), 30, 30, Vector2{0.5f, 0.5f}, Color::createGreen());

    // Drawing arc
    primitiveBatch->drawArc(
        Vector2{0.0f, 100.0f}, 40.0f,
        math::toRadians(0.0f), math::toRadians(270.0f * Easings::EaseSine::InOut(t)),
        80, Color::createYellow());

    // Drawing triangle
    primitiveBatch->drawTriangle(
        Vector2{0.0f, -40.0f}, Vector2{40.0f, 0.0f}, Vector2{40.0f, -40.0f},
        Color::createBlack(), Color::createGreen(), Color::createRed());

    primitiveBatch->end();

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
