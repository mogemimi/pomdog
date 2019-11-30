#include "EditorGUITest.hpp"

namespace FeatureShowcase {

EditorGUITest::EditorGUITest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void EditorGUITest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->ToString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 32.0f, 32.0f);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    drawingContext = std::make_unique<UI::DrawingContext>(graphicsDevice, *assets);

    auto window = gameHost->GetWindow();
    hierarchy = std::make_unique<UI::UIElementHierarchy>(window, gameHost->GetKeyboard());

    auto dispatcher = hierarchy->GetDispatcher();
    auto stackPanel = std::make_shared<UI::StackPanel>(dispatcher, 150, 170);
    stackPanel->SetTransform(Matrix3x2::CreateTranslation(Vector2{5, 200}));
    hierarchy->AddChild(stackPanel);

    {
        auto navigator = std::make_shared<UI::DebugNavigator>(dispatcher, gameHost->GetClock());
        stackPanel->AddChild(navigator);
    }
    {
        auto horizontalLayout = std::make_shared<UI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<UI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Slider");
        horizontalLayout->AddChild(textBlock);

        auto slider = std::make_shared<UI::Slider>(dispatcher, 0.0, 100.0);
        horizontalLayout->AddChild(slider);
        
        horizontalLayout->SetStretchFactor(textBlock, 1);
        horizontalLayout->SetStretchFactor(slider, 2);
    }
    {
        auto horizontalLayout = std::make_shared<UI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<UI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Toggle");
        horizontalLayout->AddChild(textBlock);

        auto toggleSwitch = std::make_shared<UI::ToggleSwitch>(dispatcher);
        horizontalLayout->AddChild(toggleSwitch);
    }
    {
        auto horizontalLayout = std::make_shared<UI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<UI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("Button");
        horizontalLayout->AddChild(textBlock);

        auto button = std::make_shared<UI::PushButton>(dispatcher);
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
        auto horizontalLayout = std::make_shared<UI::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<UI::TextBlock>(dispatcher);
        textBlock->SetColor(Color::White);
        textBlock->SetText("TextField");
        horizontalLayout->AddChild(textBlock);

        textField = std::make_shared<UI::TextField>(dispatcher);
        textField->SetPlaceholderText("Message");
        connect(textField->TextChanged, [this]() {
            propertyText1 = textField->GetText();
        });
        horizontalLayout->AddChild(textField);
    }

    auto treeView = std::make_shared<UI::TreeView>(dispatcher);
    treeView->SetText("Tree");
    stackPanel->AddChild(treeView);
    {
        auto slider = std::make_shared<UI::Slider>(dispatcher, 0.0, 100.0);
        treeView->AddChild(slider);
        
        auto toggleSwitch = std::make_shared<UI::ToggleSwitch>(dispatcher);
        treeView->AddChild(toggleSwitch);
    }
}

void EditorGUITest::Update()
{
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
    pass.ClearStencil = 0;
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
