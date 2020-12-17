#include "EditorGUITest.hpp"

namespace FeatureShowcase {

EditorGUITest::EditorGUITest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> EditorGUITest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return Errors::Wrap(std::move(fontErr), "failed to load a font file");
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 32.0f, 32.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    drawingContext = std::make_unique<GUI::DrawingContext>(graphicsDevice, *assets);

    auto window = gameHost->GetWindow();
    hierarchy = std::make_unique<GUI::WidgetHierarchy>(window, gameHost->GetKeyboard());

    auto dispatcher = hierarchy->GetDispatcher();
    {
        auto stackPanel = std::make_shared<GUI::StackPanel>(dispatcher, 150, 170);
        stackPanel->SetPosition(Point2D{200, 250});
        stackPanel->SetPadding(GUI::Thickness{2, 2, 2, 8});
        hierarchy->AddChild(stackPanel);

        auto scrollView = std::make_shared<GUI::ScrollView>(dispatcher, 150, 170);
        stackPanel->AddChild(scrollView);

        auto verticalLayout = std::make_shared<GUI::VerticalLayout>(dispatcher, 150, 170);
        verticalLayout->SetLayoutSpacing(10);
        scrollView->SetWidget(verticalLayout);

        {
            auto treeView = std::make_shared<GUI::TreeView>(dispatcher);
            treeView->SetText("Tree");
            verticalLayout->AddChild(treeView);
            {
                auto slider = std::make_shared<GUI::Slider>(dispatcher, 0.0, 100.0);
                treeView->AddChild(slider);

                auto toggleSwitch = std::make_shared<GUI::ToggleSwitch>(dispatcher);
                treeView->AddChild(toggleSwitch);
            }
        }

        for (int i = 0; i < 10; i++) {
            auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 100, 10);
            verticalLayout->AddChild(horizontalLayout);

            auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
            textBlock->SetColor(Color::White);
            textBlock->SetText("Vector3");
            horizontalLayout->AddChild(textBlock);

            auto vec3Field = std::make_shared<GUI::Vector3Field>(dispatcher);
            connect(vec3Field->ValueChanged, [this](const Vector3& value) {
                propertyText1 = StringHelper::Format("%f, %f, %f", value.X, value.Y, value.Z);
            });
            horizontalLayout->AddChild(vec3Field);

            horizontalLayout->SetStretchFactor(0, 2);
            horizontalLayout->SetStretchFactor(1, 3);
        }

        {
            auto treeView = std::make_shared<GUI::TreeView>(dispatcher);
            treeView->SetText("Tree");
            verticalLayout->AddChild(treeView);
            {
                auto slider = std::make_shared<GUI::Slider>(dispatcher, 0.0, 100.0);
                treeView->AddChild(slider);

                auto toggleSwitch = std::make_shared<GUI::ToggleSwitch>(dispatcher);
                treeView->AddChild(toggleSwitch);
            }
        }

        scrollView->ScrollToTop();
    }

    {
        auto stackPanel = std::make_shared<GUI::StackPanel>(dispatcher, 150, 170);
        stackPanel->SetPosition(Point2D{400, 250});
        stackPanel->SetPadding(GUI::Thickness{2, 0, 2, 0});
        hierarchy->AddChild(stackPanel);

        auto scrollView = std::make_shared<GUI::ScrollView>(dispatcher, 150, 170);
        stackPanel->AddChild(scrollView);

        auto listView = std::make_shared<GUI::ListView>(dispatcher, 150, 170);
        //listView->SetLayoutSpacing(10);
        scrollView->SetWidget(listView);

        for (int i = 0; i < 40; ++i) {
            auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
            textBlock->SetColor(Color::White);
            textBlock->SetText("Text " + std::to_string(i));
            listView->AddChild(textBlock);
        }

        scrollView->ScrollToTop();
    }

//    {
//        auto contextMenu = std::make_shared<GUI::ContextMenu>(dispatcher, 150, 170);
//        contextMenu->SetPosition(Point2D{450, 250});
//        hierarchy->AddChild(contextMenu);
//
//        {
//            auto button = std::make_shared<GUI::ContextMenuButton>(dispatcher);
//            button->SetText("Copy");
//            contextMenu->AddChild(button);
//        }
//        {
//            auto button = std::make_shared<GUI::ContextMenuButton>(dispatcher);
//            button->SetText("Cut");
//            contextMenu->AddChild(button);
//        }
//        {
//            auto button = std::make_shared<GUI::ContextMenuButton>(dispatcher);
//            button->SetText("Paste");
//            contextMenu->AddChild(button);
//        }
//    }

    auto stackPanel = std::make_shared<GUI::StackPanel>(dispatcher, 170, 170);
    stackPanel->SetPosition(Point2D{5, 260});
    hierarchy->AddChild(stackPanel);

    {
        auto navigator = std::make_shared<GUI::DebugNavigator>(dispatcher, gameHost->GetClock());
        stackPanel->AddChild(navigator);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Slider");
        horizontalLayout->AddChild(textBlock);

        auto slider = std::make_shared<GUI::Slider>(dispatcher, 0.0, 100.0);
        horizontalLayout->AddChild(slider);

        horizontalLayout->SetStretchFactor(textBlock, 1);
        horizontalLayout->SetStretchFactor(slider, 2);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Toggle");
        horizontalLayout->AddChild(textBlock);

        auto toggleSwitch = std::make_shared<GUI::ToggleSwitch>(dispatcher);
        horizontalLayout->AddChild(toggleSwitch);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("CheckBox");
        horizontalLayout->AddChild(textBlock);

        auto checkBox = std::make_shared<GUI::CheckBox>(dispatcher);
        horizontalLayout->AddChild(checkBox);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("PopupMenu");
        horizontalLayout->AddChild(textBlock);

        auto popupMenu = std::make_shared<GUI::PopupMenu>(dispatcher);
        popupMenu->AddItem("Blue");
        popupMenu->AddItem("Yellow");
        popupMenu->AddItem("Red");
        connect(popupMenu->CurrentIndexChanged, [this, p = popupMenu.get()]([[maybe_unused]] int index) {
            propertyText2 += "\n";
            propertyText2 += p->GetText();
        });
        horizontalLayout->AddChild(popupMenu);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Button");
        horizontalLayout->AddChild(textBlock);

        auto button = std::make_shared<GUI::PushButton>(dispatcher);
        button->SetHorizontalAlignment(GUI::HorizontalAlignment::Stretch);
        button->SetText("Submit");
        connect(button->Click, [this]() {
            if (textField->GetText().empty()) {
                propertyText1 = "empty message!";
                return;
            }
            propertyText2 += "\n";
            propertyText2 += textField->GetText();
            textField->SetText("");
            propertyText1 = "";
        });
        horizontalLayout->AddChild(button);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("TextField");
        horizontalLayout->AddChild(textBlock);

        textField = std::make_shared<GUI::TextField>(dispatcher);
        textField->SetPlaceholderText("Message");
        connect(textField->TextChanged, [this]() {
            propertyText1 = textField->GetText();
        });
        horizontalLayout->AddChild(textField);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("FloatField");
        horizontalLayout->AddChild(textBlock);

        auto floatField = std::make_shared<GUI::FloatField>(dispatcher);
        floatField->SetPlaceholderText("Scale");
        connect(floatField->ValueChanged, [this](double value) {
            propertyText1 = std::to_string(value);
        });
        horizontalLayout->AddChild(floatField);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("IntField");
        horizontalLayout->AddChild(textBlock);

        auto intField = std::make_shared<GUI::IntField>(dispatcher);
        intField->SetPlaceholderText("Count");
        connect(intField->ValueChanged, [this](int value) {
            propertyText1 = std::to_string(value);
        });
        horizontalLayout->AddChild(intField);
    }
    {
        auto horizontalLayout = std::make_shared<GUI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<GUI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Vector3");
        horizontalLayout->AddChild(textBlock);

        auto vec3Field = std::make_shared<GUI::Vector3Field>(dispatcher);
        connect(vec3Field->ValueChanged, [this](const Vector3& value) {
            propertyText1 = StringHelper::Format("%f, %f, %f", value.X, value.Y, value.Z);
        });
        horizontalLayout->AddChild(vec3Field);
    }

    auto treeView = std::make_shared<GUI::TreeView>(dispatcher);
    treeView->SetText("Tree");
    stackPanel->AddChild(treeView);
    {
        auto slider = std::make_shared<GUI::Slider>(dispatcher, 0.0, 100.0);
        treeView->AddChild(slider);

        auto toggleSwitch = std::make_shared<GUI::ToggleSwitch>(dispatcher);
        treeView->AddChild(toggleSwitch);
    }

    return nullptr;
}

void EditorGUITest::Update()
{
    hierarchy->Update();

    if (auto mouse = gameHost->GetMouse(); mouse != nullptr) {
        hierarchy->Touch(mouse->GetState());
    }

    auto clock = gameHost->GetClock();
    hierarchy->UpdateAnimation(clock->GetFrameDuration());
}

void EditorGUITest::Draw()
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
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);
    spriteFont->Draw(*spriteBatch, propertyText1, Vector2::Zero, Color{255, 255, 255, 190}, 0.0f, Vector2{0.0f, 1.0f}, 1.0f);
    spriteFont->Draw(*spriteBatch, propertyText2, Vector2::Zero, Color::White, 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    spriteBatch->End();

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
