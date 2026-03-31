#include "editor_gui_test.h"
#include "pomdog/utility/string_format.h"

namespace feature_showcase {

EditorGUITest::EditorGUITest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
EditorGUITest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Default);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    constexpr bool useSDF = false;

    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 32.0f, 32.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }
    spriteFont_->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto drawingContextErr = drawingContext_->initialize(graphicsDevice_, fs_); drawingContextErr != nullptr) {
        return errors::wrap(std::move(drawingContextErr), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->GetDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 150, 170);
        stackPanel->SetPosition(Point2D{200, 250});
        stackPanel->SetPadding(gui::Thickness{2, 2, 2, 8});
        hierarchy_->AddChild(stackPanel);

        auto scrollView = std::make_shared<gui::ScrollView>(dispatcher, 150, 170);
        stackPanel->AddChild(scrollView);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 150, 170);
        verticalLayout->SetLayoutSpacing(10);
        scrollView->SetWidget(verticalLayout);

        {
            auto treeView = std::make_shared<gui::TreeView>(dispatcher);
            treeView->SetText("Tree");
            verticalLayout->AddChild(treeView);
            {
                auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 100.0);
                treeView->AddChild(slider);

                auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
                treeView->AddChild(toggleSwitch);
            }
        }

        for (int i = 0; i < 10; i++) {
            auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 100, 10);
            verticalLayout->AddChild(horizontalLayout);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->SetColor(Color::createWhite());
            textBlock->SetText("Vector3");
            horizontalLayout->AddChild(textBlock);

            auto vec3Field = std::make_shared<gui::Vector3Field>(dispatcher);
            connect_(vec3Field->ValueChanged, [this](const Vector3& value) {
                propertyText1_ = pomdog::format("{}, {}, {}", value.x, value.y, value.z);
            });
            horizontalLayout->AddChild(vec3Field);

            horizontalLayout->SetStretchFactor(0, 2);
            horizontalLayout->SetStretchFactor(1, 3);
        }

        {
            auto treeView = std::make_shared<gui::TreeView>(dispatcher);
            treeView->SetText("Tree");
            verticalLayout->AddChild(treeView);
            {
                auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 100.0);
                treeView->AddChild(slider);

                auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
                treeView->AddChild(toggleSwitch);
            }
        }

        scrollView->ScrollToTop();
    }

    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 150, 170);
        stackPanel->SetPosition(Point2D{400, 250});
        stackPanel->SetPadding(gui::Thickness{2, 0, 2, 0});
        hierarchy_->AddChild(stackPanel);

        auto scrollView = std::make_shared<gui::ScrollView>(dispatcher, 150, 170);
        stackPanel->AddChild(scrollView);

        auto listView = std::make_shared<gui::ListView>(dispatcher, 150, 170);
        //listView->SetLayoutSpacing(10);
        scrollView->SetWidget(listView);

        for (int i = 0; i < 40; ++i) {
            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->SetColor(Color::createWhite());
            textBlock->SetText("Text " + std::to_string(i));
            listView->AddChild(textBlock);
        }

        scrollView->ScrollToTop();
    }

    // {
    //     auto contextMenu = std::make_shared<gui::ContextMenu>(dispatcher, 150, 170);
    //     contextMenu->SetPosition(Point2D{450, 250});
    //     hierarchy->AddChild(contextMenu);
    //
    //     {
    //         auto button = std::make_shared<gui::ContextMenuButton>(dispatcher);
    //         button->SetText("Copy");
    //         contextMenu->AddChild(button);
    //     }
    //     {
    //         auto button = std::make_shared<gui::ContextMenuButton>(dispatcher);
    //         button->SetText("Cut");
    //         contextMenu->AddChild(button);
    //     }
    //     {
    //         auto button = std::make_shared<gui::ContextMenuButton>(dispatcher);
    //         button->SetText("Paste");
    //         contextMenu->AddChild(button);
    //     }
    // }

    auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 170, 170);
    stackPanel->SetPosition(Point2D{5, 260});
    hierarchy_->AddChild(stackPanel);

    {
        auto navigator = std::make_shared<gui::DebugNavigator>(dispatcher, gameHost_->getClock());
        stackPanel->AddChild(navigator);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("Slider");
        horizontalLayout->AddChild(textBlock);

        auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 100.0);
        horizontalLayout->AddChild(slider);

        horizontalLayout->SetStretchFactor(textBlock, 1);
        horizontalLayout->SetStretchFactor(slider, 2);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("Toggle");
        horizontalLayout->AddChild(textBlock);

        auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
        horizontalLayout->AddChild(toggleSwitch);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("CheckBox");
        horizontalLayout->AddChild(textBlock);

        auto checkBox = std::make_shared<gui::CheckBox>(dispatcher);
        horizontalLayout->AddChild(checkBox);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("PopupMenu");
        horizontalLayout->AddChild(textBlock);

        auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
        popupMenu->AddItem("Blue");
        popupMenu->AddItem("Yellow");
        popupMenu->AddItem("Red");
        connect_(popupMenu->CurrentIndexChanged, [this, p = popupMenu.get()]([[maybe_unused]] int index) {
            propertyText2_ += "\n";
            propertyText2_ += p->GetText();
        });
        horizontalLayout->AddChild(popupMenu);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("Button");
        horizontalLayout->AddChild(textBlock);

        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->SetHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->SetText("Submit");
        connect_(button->Click, [this]() {
            if (textField_->GetText().empty()) {
                propertyText1_ = "empty message!";
                return;
            }
            propertyText2_ += "\n";
            propertyText2_ += textField_->GetText();
            textField_->SetText("");
            propertyText1_ = "";
        });
        horizontalLayout->AddChild(button);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("TextField");
        horizontalLayout->AddChild(textBlock);

        textField_ = std::make_shared<gui::TextField>(dispatcher);
        textField_->SetPlaceholderText("Message");
        connect_(textField_->TextChanged, [this]() {
            propertyText1_ = textField_->GetText();
        });
        horizontalLayout->AddChild(textField_);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("FloatField");
        horizontalLayout->AddChild(textBlock);

        auto floatField = std::make_shared<gui::FloatField>(dispatcher);
        floatField->SetPlaceholderText("Scale");
        connect_(floatField->ValueChanged, [this](double value) {
            propertyText1_ = std::to_string(value);
        });
        horizontalLayout->AddChild(floatField);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("IntField");
        horizontalLayout->AddChild(textBlock);

        auto intField = std::make_shared<gui::IntField>(dispatcher);
        intField->SetPlaceholderText("Count");
        connect_(intField->ValueChanged, [this](int value) {
            propertyText1_ = std::to_string(value);
        });
        horizontalLayout->AddChild(intField);
    }
    {
        auto horizontalLayout = std::make_shared<gui::HorizontalLayout>(dispatcher, 140, 10);
        stackPanel->AddChild(horizontalLayout);

        auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
        textBlock->SetColor(Color::createWhite());
        textBlock->SetText("Vector3");
        horizontalLayout->AddChild(textBlock);

        auto vec3Field = std::make_shared<gui::Vector3Field>(dispatcher);
        connect_(vec3Field->ValueChanged, [this](const Vector3& value) {
            propertyText1_ = pomdog::format("{}, {}, {}", value.x, value.y, value.z);
        });
        horizontalLayout->AddChild(vec3Field);
    }

    auto treeView = std::make_shared<gui::TreeView>(dispatcher);
    treeView->SetText("Tree");
    stackPanel->AddChild(treeView);
    {
        auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 100.0);
        treeView->AddChild(slider);

        auto toggleSwitch = std::make_shared<gui::ToggleSwitch>(dispatcher);
        treeView->AddChild(toggleSwitch);
    }

    return nullptr;
}

void EditorGUITest::update()
{
    hierarchy_->Update();

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->Touch(mouse->getState());
    }

    auto clock = gameHost_->getClock();
    hierarchy_->UpdateAnimation(clock->getFrameDuration());
}

void EditorGUITest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

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
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch_->begin(commandList_, primitivePipeline_, projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->end();

    spriteBatch_->begin(commandList_, spritePipeline_, projectionMatrix);
    spriteFont_->draw(*spriteBatch_, propertyText1_, Vector2::createZero(), Color{255, 255, 255, 190}, 0.0f, Vector2{0.0f, 1.0f}, 1.0f);
    spriteFont_->draw(*spriteBatch_, propertyText2_, Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    spriteBatch_->end();

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->Reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
    drawingContext_->BeginDraw(commandList_, viewMatrix * projectionMatrix);
    hierarchy_->Draw(*drawingContext_);
    drawingContext_->EndDraw();

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
