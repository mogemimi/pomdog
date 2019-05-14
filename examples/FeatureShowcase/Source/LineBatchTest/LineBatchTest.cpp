#include "LineBatchTest.hpp"
#include <Pomdog/Experimental/Tween/EasingHelper.hpp>

namespace FeatureShowcase {

LineBatchTest::LineBatchTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void LineBatchTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    lineBatch = std::make_shared<LineBatch>(graphicsDevice, *assets);
    lineBatch2 = std::make_shared<LineBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.1);

    path.push_back(Vector2::Zero);

    auto mouse = gameHost->GetMouse();
    connect(mouse->ButtonDown, [this](MouseButtons button) {
        auto state = gameHost->GetMouse()->GetState();
        auto width = gameHost->GetWindow()->GetClientBounds().Width;
        auto height = gameHost->GetWindow()->GetClientBounds().Height;
        if (button == MouseButtons::Left) {
            path.back() = MathHelper::ToVector2(Point2D{state.Position.X - (width / 2), (height / 2) - state.Position.Y});
            path.push_back(path.back());
        }
        if (button == MouseButtons::Right) {
            path.clear();
            path.push_back(Vector2::Zero);
        }
    });
}

void LineBatchTest::Update()
{
    auto state = gameHost->GetMouse()->GetState();
    auto width = gameHost->GetWindow()->GetClientBounds().Width;
    auto height = gameHost->GetWindow()->GetClientBounds().Height;
    path.back() = MathHelper::ToVector2(Point2D{state.Position.X - (width / 2), (height / 2) - state.Position.Y});
}

void LineBatchTest::Draw()
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

    {
        auto world = Matrix4x4::CreateScale(0.1f) *
            Matrix4x4::CreateRotationX(Math::PiOver4<float>) *
            Matrix4x4::CreateRotationY(std::sin( Math::TwoPi<float> * static_cast<float>(timer->GetTotalTime().count())));
        auto view =  Matrix4x4::CreateTranslation(Vector3{0.0f, 0.0f, 50.0f});

        auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
            Math::PiOver4<float>,
            static_cast<float>(presentationParameters.BackBufferWidth) / static_cast<float>(presentationParameters.BackBufferHeight),
            0.0001f,
            500.0f);

        lineBatch2->Begin(commandList, world * view * projectionMatrix);
        lineBatch2->DrawSphere(Vector3::Zero, 100.0f, Color::Blue, 16);
        lineBatch2->End();
    }

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        0.0f,
        100.0f);

    lineBatch->Begin(commandList, projectionMatrix);
    for (size_t i = 1; i < path.size(); i++) {
        auto start = path[i - 1];
        auto end = path[i];
        lineBatch->DrawLine(start, end, Color{255, 255, 255, 160});
    }
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

} // namespace FeatureShowcase
