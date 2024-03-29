#include "polyline_drawing_test.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace feature_showcase {

PolylineDrawingTest::PolylineDrawingTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> PolylineDrawingTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    lineBatch = std::make_shared<PolylineBatch>(graphicsDevice, *assets);

    auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons button) {
        if (button == MouseButtons::Left) {
            polylineClosed = false;
        }
        if (button == MouseButtons::Right) {
            path.clear();
        }
    });
    connect(mouse->ButtonUp, [this](MouseButtons button) {
        if (button == MouseButtons::Left) {
            polylineClosed = true;
        }
    });
    connect(mouse->ScrollWheel, [this](int32_t delta) {
        lineWidth = std::clamp(lineWidth + static_cast<float>(delta) * 0.1f, 0.5f, 40.0f);
    });

    return nullptr;
}

void PolylineDrawingTest::update()
{
    const auto mouseState = gameHost->getMouse()->getState();
    const auto clientBounds = gameHost->getWindow()->getClientBounds();

    const auto width = clientBounds.width;
    const auto height = clientBounds.height;
    const auto pos = math::toVector2(Point2D{mouseState.position.x - (width / 2), (height / 2) - mouseState.position.y});

    if (mouseState.leftButton == ButtonState::Pressed) {
        if (path.empty()) {
            path.push_back(pos);
            path.push_back(pos);
        }
        else if (math::distanceSquared(pos, path[path.size() - 2]) > 10.0f) {
            path.back() = pos;
            path.push_back(pos);
        }
    }

    if (!path.empty()) {
        path.back() = pos;
    }
}

void PolylineDrawingTest::draw()
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

    float thickness = lineWidth / static_cast<float>(presentationParameters.backBufferWidth);

    lineBatch->begin(commandList, projectionMatrix);
    lineBatch->drawPath(path, polylineClosed, Color{255, 255, 255, 200}, thickness);
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
