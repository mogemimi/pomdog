#include "polyline_drawing_test.h"
#include "pomdog/experimental/tween/easing_helper.hpp"

namespace feature_showcase {

PolylineDrawingTest::PolylineDrawingTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> PolylineDrawingTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    lineBatch = std::make_shared<PolylineBatch>(graphicsDevice, *assets);

    auto mouse = gameHost->GetMouse();
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

void PolylineDrawingTest::Update()
{
    const auto mouseState = gameHost->GetMouse()->GetState();

    const auto width = gameHost->GetWindow()->GetClientBounds().Width;
    const auto height = gameHost->GetWindow()->GetClientBounds().Height;
    const auto pos = math::ToVector2(Point2D{mouseState.Position.X - (width / 2), (height / 2) - mouseState.Position.Y});

    if (mouseState.LeftButton == ButtonState::Pressed) {
        if (path.empty()) {
            path.push_back(pos);
            path.push_back(pos);
        }
        else if (math::DistanceSquared(pos, path[path.size() - 2]) > 10.0f) {
            path.back() = pos;
            path.push_back(pos);
        }
    }

    if (!path.empty()) {
        path.back() = pos;
    }
}

void PolylineDrawingTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    float thickness = lineWidth / static_cast<float>(presentationParameters.BackBufferWidth);

    lineBatch->Begin(commandList, projectionMatrix);
    lineBatch->DrawPath(path, polylineClosed, Color{255, 255, 255, 200}, thickness);
    lineBatch->End();

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
