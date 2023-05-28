#include "gui_splitter_test.h"

namespace feature_showcase {

GUISplitterTest::GUISplitterTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> GUISplitterTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    drawingContext = std::make_unique<gui::DrawingContext>(graphicsDevice, *assets);

    auto window = gameHost->getWindow();
    hierarchy = std::make_unique<gui::WidgetHierarchy>(window, gameHost->getKeyboard());

    auto dispatcher = hierarchy->GetDispatcher();

    auto clientBounds = window->getClientBounds();
    auto splitter = std::make_shared<gui::HorizontalSplitter>(dispatcher, clientBounds.width, clientBounds.height);
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

void GUISplitterTest::update()
{
    hierarchy->Update();

    if (auto mouse = gameHost->getMouse(); mouse != nullptr) {
        hierarchy->Touch(mouse->GetState());
    }

    auto clock = gameHost->getClock();
    hierarchy->UpdateAnimation(clock->GetFrameDuration());
}

void GUISplitterTest::draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext->Reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
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
