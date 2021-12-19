#include "GUISplitterTest.hpp"

namespace feature_showcase {

GUISplitterTest::GUISplitterTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> GUISplitterTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    drawingContext = std::make_unique<gui::DrawingContext>(graphicsDevice, *assets);

    auto window = gameHost->GetWindow();
    hierarchy = std::make_unique<gui::WidgetHierarchy>(window, gameHost->GetKeyboard());

    auto dispatcher = hierarchy->GetDispatcher();

    auto clientBounds = window->GetClientBounds();
    auto splitter = std::make_shared<gui::HorizontalSplitter>(dispatcher, clientBounds.Width, clientBounds.Height);
    splitter->SetPosition(Point2D{0, 0});
    splitter->SetLayoutSpacing(1);
    hierarchy->AddChild(splitter);

    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->SetSize(100, 70);
        button->SetHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->SetText("LayoutSpacing = 0");
        connect(button->Click, [splitter] {
            splitter->SetLayoutSpacing(0);
            splitter->DoLayout();
        });
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 100);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->SetHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->SetText("LayoutSpacing = 1");
        connect(button->Click, [splitter] {
            splitter->SetLayoutSpacing(1);
            splitter->DoLayout();
        });
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 100);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->SetHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->SetText("LayoutSpacing = 2");
        connect(button->Click, [splitter] {
            splitter->SetLayoutSpacing(2);
            splitter->DoLayout();
        });
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 150);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->SetHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->SetVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->SetText("LayoutSpacing = 5");
        connect(button->Click, [splitter] {
            splitter->SetLayoutSpacing(5);
            splitter->DoLayout();
        });
        splitter->AddChild(button);
        splitter->SetMinimumWidth(button, 100);
    }

    return nullptr;
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
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
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

} // namespace feature_showcase
