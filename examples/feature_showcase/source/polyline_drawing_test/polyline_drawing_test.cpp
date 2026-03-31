#include "polyline_drawing_test.h"
#include "pomdog/experimental/tween/easing_helper.h"

namespace feature_showcase {

PolylineDrawingTest::PolylineDrawingTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
PolylineDrawingTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPolylinePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PolylinePipeline");
    }
    else {
        linePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPolylineBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PolylineBatch");
    }
    else {
        lineBatch_ = std::move(p);
    }

    auto mouse = gameHost_->getMouse();
    connect_(mouse->ButtonDown, [this](MouseButtons button) {
        if (button == MouseButtons::Left) {
            polylineClosed_ = false;
        }
        if (button == MouseButtons::Right) {
            path_.clear();
        }
    });
    connect_(mouse->ButtonUp, [this](MouseButtons button) {
        if (button == MouseButtons::Left) {
            polylineClosed_ = true;
        }
    });
    connect_(mouse->ScrollWheel, [this](int32_t delta) {
        lineWidth_ = std::clamp(lineWidth_ + static_cast<float>(delta) * 0.1f, 0.5f, 40.0f);
    });

    return nullptr;
}

void PolylineDrawingTest::update()
{
    const auto mouseState = gameHost_->getMouse()->getState();
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

    const auto width = clientBounds.width;
    const auto height = clientBounds.height;
    const auto pos = math::toVector2(Point2D{mouseState.position.x - (width / 2), (height / 2) - mouseState.position.y});

    if (mouseState.leftButton == ButtonState::Down) {
        if (path_.empty()) {
            path_.push_back(pos);
            path_.push_back(pos);
        }
        else if (math::distanceSquared(pos, path_[path_.size() - 2]) > 10.0f) {
            path_.back() = pos;
            path_.push_back(pos);
        }
    }

    if (!path_.empty()) {
        path_.back() = pos;
    }
}

void PolylineDrawingTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    float thickness = lineWidth_ / static_cast<float>(presentationParameters.backBufferWidth);

    lineBatch_->begin(commandList_, linePipeline_, projectionMatrix);
    lineBatch_->drawPath(path_, polylineClosed_, Color{255, 255, 255, 200}, thickness);
    lineBatch_->end();

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
