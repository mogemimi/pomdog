#include "gui_splitter_test.h"

namespace feature_showcase {

GUISplitterTest::GUISplitterTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
GUISplitterTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto drawingContextErr = drawingContext_->initialize(graphicsDevice_, fs_); drawingContextErr != nullptr) {
        return errors::wrap(std::move(drawingContextErr), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();

    auto clientBounds = window->getClientBounds();
    auto splitter = std::make_shared<gui::HorizontalSplitter>(dispatcher, clientBounds.width, clientBounds.height);
    splitter->setPosition(Point2D{0, 0});
    splitter->setLayoutSpacing(1);
    hierarchy_->addChild(splitter);

    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->setSize(100, 70);
        button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->setVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->setText("LayoutSpacing = 0");
        connect_(button->Click, [splitter] {
            splitter->setLayoutSpacing(0);
            splitter->doLayout();
        });
        splitter->addChild(button);
        splitter->setMinimumWidth(button, 100);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->setVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->setText("LayoutSpacing = 1");
        connect_(button->Click, [splitter] {
            splitter->setLayoutSpacing(1);
            splitter->doLayout();
        });
        splitter->addChild(button);
        splitter->setMinimumWidth(button, 100);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->setVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->setText("LayoutSpacing = 2");
        connect_(button->Click, [splitter] {
            splitter->setLayoutSpacing(2);
            splitter->doLayout();
        });
        splitter->addChild(button);
        splitter->setMinimumWidth(button, 150);
    }
    {
        auto button = std::make_shared<gui::PushButton>(dispatcher);
        button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
        button->setVerticalAlignment(gui::VerticalAlignment::Stretch);
        button->setText("LayoutSpacing = 5");
        connect_(button->Click, [splitter] {
            splitter->setLayoutSpacing(5);
            splitter->doLayout();
        });
        splitter->addChild(button);
        splitter->setMinimumWidth(button, 100);
    }

    return nullptr;
}

void GUISplitterTest::update()
{
    hierarchy_->update();

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(*mouse);
    }

    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void GUISplitterTest::draw()
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

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
    drawingContext_->beginDraw(commandList_, viewMatrix * projectionMatrix);
    hierarchy_->draw(*drawingContext_);
    drawingContext_->endDraw();

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
