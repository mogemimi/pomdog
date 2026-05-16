#include "line_batch_test.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

LineBatchTest::LineBatchTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
LineBatchTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createLinePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LinePipeline");
    }
    else {
        linePipeline_ = std::move(p);
    }
    if (auto [p, err] = createLineBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LineBatch");
    }
    else {
        lineBatch_ = std::move(p);
    }
    if (auto [p, err] = createLineBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LineBatch");
    }
    else {
        lineBatch2_ = std::move(p);
    }
    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.1);

    path_.push_back(Vector2::createZero());

    return nullptr;
}

void LineBatchTest::update()
{
    const auto mouse = gameHost_->getMouse();
    const auto window = gameHost_->getWindow();
    const auto clientBounds = window->getClientBounds();
    const auto width = clientBounds.width;
    const auto height = clientBounds.height;
    const auto mousePos = mouse->getPosition();
    path_.back() = math::toVector2(Point2D{mousePos.x - (width / 2), (height / 2) - mousePos.y});

    const bool leftDown = mouse->isButtonDown(MouseButtons::Left);
    if (leftDown && !wasLeftMouseDown_) {
        path_.push_back(path_.back());
    }
    wasLeftMouseDown_ = leftDown;

    const bool rightDown = mouse->isButtonDown(MouseButtons::Right);
    if (rightDown && !wasRightMouseDown_) {
        path_.clear();
        path_.push_back(Vector2::createZero());
    }
    wasRightMouseDown_ = rightDown;
}

void LineBatchTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    {
        auto world = Matrix4x4::createScale(0.1f) *
                     Matrix4x4::createRotationX(math::PiOver4<f32>) *
                     Matrix4x4::createRotationY(std::sin(math::TwoPi<f32> * static_cast<f32>(timer_->getTotalTime().count())));
        auto view = Matrix4x4::createTranslation(Vector3{0.0f, 0.0f, 50.0f});

        auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
            math::PiOver4<f32>,
            static_cast<f32>(presentationParameters.backBufferWidth) / static_cast<f32>(presentationParameters.backBufferHeight),
            0.0001f,
            500.0f);

        lineBatch2_->reset();
        lineBatch2_->setTransform(world * view * projectionMatrix);
        lineBatch2_->drawSphere(Vector3::createZero(), 100.0f, Color::createBlue(), 16);
        lineBatch2_->flush(commandList_, linePipeline_);
        lineBatch2_->submit(graphicsDevice_);
    }

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    lineBatch_->reset();
    lineBatch_->setTransform(projectionMatrix);
    for (size_t i = 1; i < path_.size(); i++) {
        auto start = path_[i - 1];
        auto end = path_[i];
        lineBatch_->drawLine(start, end, Color{255, 255, 255, 160});
    }
    lineBatch_->flush(commandList_, linePipeline_);
    lineBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
