#include "line_batch_test.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace feature_showcase {

LineBatchTest::LineBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> LineBatchTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    lineBatch = std::make_shared<LineBatch>(graphicsDevice, *assets);
    lineBatch2 = std::make_shared<LineBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->setInterval(std::chrono::seconds(1));
    timer->setScale(0.1);

    path.push_back(Vector2::createZero());

    const auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons button) {
        const auto window = gameHost->getWindow();
        const auto clientBounds = window->getClientBounds();
        const auto mouseState = gameHost->getMouse()->getState();
        const auto width = clientBounds.width;
        const auto height = clientBounds.height;
        if (button == MouseButtons::Left) {
            path.back() = math::toVector2(Point2D{mouseState.position.x - (width / 2), (height / 2) - mouseState.position.y});
            path.push_back(path.back());
        }
        if (button == MouseButtons::Right) {
            path.clear();
            path.push_back(Vector2::createZero());
        }
    });

    return nullptr;
}

void LineBatchTest::update()
{
    const auto state = gameHost->getMouse()->getState();
    const auto window = gameHost->getWindow();
    const auto clientBounds = window->getClientBounds();
    const auto width = clientBounds.width;
    const auto height = clientBounds.height;
    path.back() = math::toVector2(Point2D{state.position.x - (width / 2), (height / 2) - state.position.y});
}

void LineBatchTest::draw()
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

    {
        auto world = Matrix4x4::createScale(0.1f) *
            Matrix4x4::createRotationX(math::PiOver4<float>) *
            Matrix4x4::createRotationY(std::sin( math::TwoPi<float> * static_cast<float>(timer->getTotalTime().count())));
        auto view = Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 50.0f});

        auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
            math::PiOver4<float>,
            static_cast<float>(presentationParameters.backBufferWidth) / static_cast<float>(presentationParameters.backBufferHeight),
            0.0001f,
            500.0f);

        lineBatch2->begin(commandList, world * view * projectionMatrix);
        lineBatch2->drawSphere(Vector3::createZero(), 100.0f, Color::createBlue(), 16);
        lineBatch2->end();
    }

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    lineBatch->begin(commandList, projectionMatrix);
    for (size_t i = 1; i < path.size(); i++) {
        auto start = path[i - 1];
        auto end = path[i];
        lineBatch->drawLine(start, end, Color{255, 255, 255, 160});
    }
    lineBatch->end();

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
