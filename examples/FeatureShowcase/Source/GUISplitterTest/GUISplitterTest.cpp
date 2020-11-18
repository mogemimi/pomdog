#include "GUISplitterTest.hpp"

namespace FeatureShowcase {

GUISplitterTest::GUISplitterTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void GUISplitterTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::get<0>(graphicsDevice->CreateGraphicsCommandList());

    drawingContext = std::make_unique<GUI::DrawingContext>(graphicsDevice, *assets);

    auto window = gameHost->GetWindow();
    hierarchy = std::make_unique<GUI::WidgetHierarchy>(window, gameHost->GetKeyboard());

    auto dispatcher = hierarchy->GetDispatcher();

    auto clientBounds = window->GetClientBounds();
    auto splitter = std::make_shared<GUI::HorizontalSplitter>(dispatcher, clientBounds.Width, clientBounds.Height);
    splitter->SetPosition(Point2D{0, 0});
    splitter->SetLayoutSpacing(2);
    hierarchy->AddChild(splitter);

    {
        auto button = std::make_shared<GUI::PushButton>(dispatcher);
        button->SetSize(button->GetWidth(), 70);
        button->SetHorizontalAlignment(GUI::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(GUI::VerticalAlignment::Stretch);
        button->SetText("A");
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 50);
    }
    {
        auto button = std::make_shared<GUI::PushButton>(dispatcher);
        button->SetHorizontalAlignment(GUI::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(GUI::VerticalAlignment::Stretch);
        button->SetText("B");
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 100);
    }
    {
        auto button = std::make_shared<GUI::PushButton>(dispatcher);
        button->SetHorizontalAlignment(GUI::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(GUI::VerticalAlignment::Stretch);
        button->SetText("C");
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 150);
    }
    {
        auto button = std::make_shared<GUI::PushButton>(dispatcher);
        button->SetHorizontalAlignment(GUI::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(GUI::VerticalAlignment::Stretch);
        button->SetText("D");
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 20);
    }
}

void GUISplitterTest::Update()
{
    hierarchy->Update();

    if (auto mouse = gameHost->GetMouse(); mouse != nullptr) {
        hierarchy->Touch(mouse->GetState());
    }

    auto clock = gameHost->GetClock();
    hierarchy->UpdateAnimation(clock->GetFrameDuration());
}

void GUISplitterTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto viewMatrix = Matrix4x4::CreateTranslation(Vector3{
        static_cast<float>(-presentationParameters.BackBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.BackBufferHeight) * 0.5f,
        0.0f});

    drawingContext->Reset(presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight);
    drawingContext->BeginDraw(commandList, viewMatrix * projectionMatrix);
    hierarchy->Draw(*drawingContext);
    drawingContext->EndDraw();

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
